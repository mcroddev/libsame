/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Michael Rodriguez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <SDL2/SDL.h>

#include "libsame/libsame.h"

#ifndef NDEBUG
void *libsame_userdata_;

_Noreturn void libsame_assert_failed(const char *const expr,
                                     const char *const file, const int line_no,
                                     void *userdata) {
  (void)expr;
  (void)file;
  (void)line_no;
  (void)userdata;

  abort();
}

#endif /* NDEBUG */

int main(void) {
  SDL_Init(SDL_INIT_AUDIO);

  SDL_AudioDeviceID dev;

  SDL_AudioSpec audio_spec;
  SDL_zero(audio_spec);

  audio_spec.freq = LIBSAME_SAMPLE_RATE;
  audio_spec.format = AUDIO_S16LSB;
  audio_spec.channels = 1;
  audio_spec.samples = LIBSAME_SAMPLES_NUM_MAX;

  dev = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);

  if (dev == 0) {
    perror("SDL_OpenAudioDevice()");
    return EXIT_FAILURE;
  }
  printf("Using %s\n", SDL_GetAudioDeviceName((int)dev, 0));

  struct libsame_header header;
  memcpy(&header.location_codes[0], "048484", sizeof("048484"));
  memcpy(&header.location_codes[1], "048024", sizeof("048024"));
  memcpy(&header.location_codes[2], LIBSAME_LOCATION_CODE_END_MARKER,
         sizeof(LIBSAME_LOCATION_CODE_END_MARKER));

  memcpy(&header.originator_time, "1172221", sizeof("1172221"));
  memcpy(&header.valid_time_period, "1000", sizeof("1000"));

  memcpy(&header.event_code, "TOR", sizeof("TOR"));
  memcpy(&header.originator_code, "WXR", sizeof("WXR"));
  memcpy(&header.callsign, "WAEB/AM ", sizeof("WAEB/AM "));

  header.attn_sig_duration = 8;

  struct libsame_gen_ctx ctx;
  memset(&ctx, 0, sizeof(ctx));

  libsame_ctx_init(&ctx, &header);

  SDL_PauseAudioDevice(dev, 0);

  while (ctx.seq_state != LIBSAME_SEQ_STATE_NUM) {
    libsame_samples_gen(&ctx);
    SDL_QueueAudio(dev, ctx.sample_data,
                   sizeof(int16_t) * LIBSAME_SAMPLES_NUM_MAX);
  }
  SDL_Delay(20000);
  return EXIT_SUCCESS;
}
