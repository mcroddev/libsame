// SPDX-License-Identifier: MIT
//
// Copyright 2023 Michael Rodriguez
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the “Software”), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <SDL.h>
#include <signal.h>

#include "libsame/libsame.h"

#ifndef NDEBUG
// Application specified userdata. For our purposes, we have no use for it.
// This is used only for assertion handling.
void *libsame_dbg_userdata_;

// If libsame was built in debug mode, it is required that a method of this
// prototype be present at link time. Instead of directly calling `assert()`,
// we give applications a chance to do something with this information before
// aborting. For this case however, if we get here, something went terribly
// wrong, and we should just abort.
_Noreturn void libsame_assert_failed(const char *const expr,
                                     const char *const file, const int line_no,
                                     void *userdata) {
  (void)userdata;

  fprintf(stderr, "libsame: assertion '%s' failed (%s:%d)\n", expr, file,
          line_no);
  abort();
}
#endif  // NDEBUG

static SDL_AudioDeviceID audio_dev_id;

// This function is called when the application terminates via the end of main()
// or a call to exit().
static void example_on_exit(void) {
  SDL_CloseAudioDevice(audio_dev_id);
  SDL_Quit();
}

// This function handled signals from the operating system.
// We use it to handle Ctrl+C (^C) input from the keyboard to terminate the
// example.
static void sig_handler(const int signum) {
  if (signum == SIGINT) {
    exit(EXIT_SUCCESS);
  }
}

// This function outputs the warning banner and gives the user a 10-second
// countdown to prepare themselves for the output.
static void user_warning_handle(void) {
  printf(
      "This example will use the default audio device as determined by SDL2 "
      "and then\n");
  printf(
      "output a valid SAME header with an 8 second long attention signal. This "
      "will\n");
  printf(
      "play at full volume, so you have 10 seconds to prepare yourself. This "
      "example is\n");
  printf(
      "also FULLY CAPABLE OF ACTIVATING EAS SYSTEMS, YOU HAVE BEEN "
      "WARNED.\n\n");
  printf("You may end this example at any time using Ctrl+C (^C).\n\n");

  unsigned int count = 10;

  while (count) {
    printf("Time remaining: %d \r", count);
    fflush(stdout);
    count--;
    SDL_Delay(1000);
  }
}

int main(void) {
  // We want to handle SIGINT (i.e., Ctrl+C) so that the user may stop this
  // example at any time.
  signal(SIGINT, sig_handler);

  atexit(example_on_exit);

  user_warning_handle();

  if (SDL_Init(SDL_INIT_AUDIO) < 0) {
    fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  SDL_AudioSpec audio_spec;
  SDL_zero(audio_spec);

  const int SAMPLE_RATE = 44100;

  audio_spec.freq = SAMPLE_RATE;
  audio_spec.format = AUDIO_S16LSB;
  audio_spec.channels = 1;
  audio_spec.samples = LIBSAME_SAMPLES_NUM_MAX;

  audio_dev_id = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
  if (audio_dev_id == 0) {
    fprintf(stderr, "SDL_OpenAudioDevice() failed: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  printf("Generation engine: %s\n", libsame_gen_engine_desc_get());

  // This structure defines the contents of the SAME header.
  const struct libsame_header header = {
      // Because there's no dynamic memory allocation, we need a sentinel value
      // to determine the end of the location codes.
      .location_codes = {"048484", "048024", LIBSAME_LOCATION_CODE_END_MARKER},
      .originator_time = "1172221",
      .valid_time_period = "1000",
      .event_code = "TOR",
      .originator_code = "WXR",
      .callsign = "WAEB/AM  ",  // The standard requires padding with spaces
      .attn_sig_duration = 8    // seconds
  };

  // This function must be called before using libsame. If using the LUT engine,
  // this populates the internal sine wave lookup table.
  libsame_init();

  // This structure handles the generation context; since we generate
  // LIBSAME_SAMPLES_NUM_MAX at a time, we need a way to keep track of where we
  // are during the generation process. It must *always* be zeroed out upon
  // declaration.
  struct libsame_gen_ctx ctx = {};

  // Initialize the generation context with our requested header. This will
  // calculate how many samples it will take for each state in the generation.
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  // Enable the audio device.
  SDL_PauseAudioDevice(audio_dev_id, 0);

  printf("Generating and playing SAME header...\n");

  // Do not stop generating audio samples until we have reached the end of the
  // possible states.
  while (ctx.seq_state != LIBSAME_SEQ_STATE_NUM) {
    // Generate LIBSAME_SAMPLES_NUM_MAX samples.
    libsame_samples_gen(&ctx);

    // Now push them to the audio device.
    if (SDL_QueueAudio(audio_dev_id, ctx.sample_data,
                       sizeof(int16_t) * LIBSAME_SAMPLES_NUM_MAX) < 0) {
      fprintf(stderr, "SDL_QueueAudio() failed: %s\n", SDL_GetError());
      return EXIT_FAILURE;
    }
  }

  // While we could calculate algorithmically how long it will take to
  // completely play the output, an estimate just is fine.
  SDL_Delay(20000);
  printf("Done!\n");

  return EXIT_SUCCESS;
}
