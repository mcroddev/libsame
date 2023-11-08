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

/// @file libsame.c
/// Defines the implementation logic of SAME header generation.
///
/// The sample rate is fixed at 44100Hz. There appears to be no good reason to
/// go above or below that value. Unfortunately, an authoritative answer is
/// not defined in the specification. Experimentation with various different
/// decoders has not shown any problems.
///
/// Single-precision floating point is enforced; double-precision is not
/// necessary, and many embedded systems do not feature a double precision FPU.
///
/// Dynamic memory allocation is forbidden; all sizes are fixed, and all the
/// upper bounds are known at compile time.
///
/// The maximum number of samples that can be generated is defined by the
/// following premises:
///
///    * The sample rate is 44100 Hz.
///
///    * There are 7 periods of silence, each lasting 1 second: 1 after each
///      message transmission (there are 3), 1 after the attention signal, and
///      1 after each End of Message (EOM) transmission (there are 3).
///
///    * There is a maximum of 268 bytes that can be transmitted during an AFSK
///      burst, which occurs three times.
///
///    * The End of Message (EOM) transmission is 20 bytes, which occurs three
///      times.
///
///    * There are a total of 6 burst transmissions; 3 for the message portion,
///      and 3 for the EOM portion.
///
///    * The maximum amount of time an attention signal can last for is 25
///      seconds.
///
///    * The duration of each bit is 1.92ms, and we must produce 520.83 bits per
///      second. This gives us a calculation of ((1.0 / 520.83) * 44100) which
///      gives us a sum of 84.672539. However, the value needs to be rounded UP
///      to 85.
///
///    * There are 8 bits in a character.
///
/// These axioms give us the following calculations:
///
///    8 bits/char * 85 samples/bit * 268 bytes * 3 bursts = + 514,080 samples
///    7 seconds * 44,100                                  = + 308,700 samples
///    25 seconds * 44,100                                 = + 1,102,500 samples
///    8 bits/char * 85 samples/bit * 20 bytes * 3 bursts  = + 40,800 samples
///                                                          = 1,966,080 samples
///                                                            -----------------
///
/// Since we use a 16-bit integer type to store the sample data, the amount of
/// space required is (2 bytes * 1,966,080 samples) = **3,932,160 bytes, or**
/// **~4 MB.**
///
/// It is not practical to use such a large value on the stack on most embedded
/// targets, and even on traditional desktop systems this would still not be
/// dangerous. Since dynamic memory allocation is forbidden, the solution is to
/// generate chunks of audio samples and then push them to the audio device
/// incrementally. In our case, we choose to generate 4,096 samples at a time.

#include "libsame/libsame.h"

#include <math.h>
#include <string.h>

#include "libsame/compiler.h"
#include "libsame/debug.h"

#define LIBSAME_PI (3.14159265358979323846264338327950288F)

LIBSAME_STATIC int16_t libsame_sin(struct libsame_gen_ctx *const restrict ctx,
                                   float *const restrict phase, const float t,
                                   const float freq) {
#if defined(LIBSAME_CONFIG_SINE_USE_LIBC)
  (void)ctx;
  (void)phase;
  return (int16_t)(sinf(LIBSAME_PI * 2 * t * freq) * INT16_MAX);
#elif defined(LIBSAME_CONFIG_SINE_USE_LUT)
  (void)t;
  LIBSAME_ASSERT(phase != NULL);

  const int16_t sample = ctx->sin_gen_lut.entries[(size_t)*phase];

  const float delta =
      (freq * LIBSAME_CONFIG_SINE_LUT_SIZE) / LIBSAME_SAMPLE_RATE;

  *phase += delta;

  while (*phase >= LIBSAME_CONFIG_SINE_LUT_SIZE) {
    *phase -= LIBSAME_CONFIG_SINE_LUT_SIZE;
  }

  return sample;
#elif defined(LIBSAME_CONFIG_SINE_USE_TAYLOR)
  return 0;
#elif defined(LIBSAME_CONFIG_SINE_USE_APP)
  return libsame_app_sin(x);
#else
#error "Unknown AFSK generation engine; implement it."
#endif
}

LIBSAME_STATIC void libsame_field_add(uint8_t *const restrict data,
                                      size_t *restrict data_size,
                                      const char *restrict const field,
                                      const size_t field_len) {
  LIBSAME_ASSERT(data != NULL);
  LIBSAME_ASSERT(data_size != NULL);
  LIBSAME_ASSERT(field != NULL);
  LIBSAME_ASSERT(field_len > 0);

  /// XXX: LIBSAME_CALLSIGN_LEN is the largest field, if this ever changes this
  /// must change as well.
  LIBSAME_ASSERT(field_len <= LIBSAME_CALLSIGN_LEN);

  memcpy(&data[*data_size], field, field_len);
  *data_size += field_len;
  data[(*data_size)++] = '-';
}

LIBSAME_STATIC void libsame_afsk_gen(struct libsame_gen_ctx *const restrict ctx,
                                     const uint8_t *const restrict data,
                                     const size_t data_size,
                                     const size_t sample_pos) {
  LIBSAME_ASSERT(ctx != NULL);
  LIBSAME_ASSERT(data != NULL);
  LIBSAME_ASSERT(data_size > 0);

  const float freq = ((data[ctx->afsk.data_pos] >> ctx->afsk.bit_pos) & 1)
                         ? LIBSAME_AFSK_MARK_FREQ
                         : LIBSAME_AFSK_SPACE_FREQ;

  const float t = (float)ctx->afsk.sample_num / LIBSAME_SAMPLE_RATE;

#if defined(LIBSAME_CONFIG_SINE_USE_LIBC)
  const int16_t sample = libsame_sin(ctx, NULL, t, freq);
#elif defined(LIBSAME_CONFIG_SINE_USE_LUT)
  const int16_t sample = libsame_sin(ctx, &ctx->afsk.phase, t, freq);
#elif defined(LIBSAME_CONFIG_SINE_USE_TAYLOR)
  ;
#else
#error "Unknown generation engine!"
#endif

  ctx->sample_data[sample_pos] = sample;

  ctx->afsk.sample_num++;

  if (ctx->afsk.sample_num >= LIBSAME_AFSK_SAMPLES_PER_BIT) {
    ctx->afsk.sample_num = 0;
    ctx->afsk.bit_pos++;

    if (ctx->afsk.bit_pos >= LIBSAME_AFSK_BITS_PER_CHAR) {
      ctx->afsk.bit_pos = 0;
      ctx->afsk.data_pos++;

      if (ctx->afsk.data_pos >= data_size) {
        // By the time we get here, we're completely done caring about the AFSK
        // state for the current state; clear it to prepare for the next one.
        memset(&ctx->afsk, 0, sizeof(ctx->afsk));
      }
    }
  }
}

LIBSAME_STATIC void libsame_silence_gen(
    struct libsame_gen_ctx *const restrict ctx, const size_t sample_pos) {
  LIBSAME_ASSERT(ctx != NULL);
  ctx->sample_data[sample_pos] = 0;
}

LIBSAME_STATIC void libsame_attn_sig_gen(
    struct libsame_gen_ctx *const restrict ctx, const size_t sample_pos) {
  LIBSAME_ASSERT(ctx != NULL);

  const float t = (float)ctx->attn_sig_sample_num / LIBSAME_SAMPLE_RATE;

#if defined(LIBSAME_CONFIG_SINE_USE_LIBC)
  const int32_t first_sample =
      libsame_sin(ctx, NULL, t, LIBSAME_ATTN_SIG_FREQ_FIRST) /
      (int32_t)sizeof(int16_t);
  const int32_t second_sample =
      libsame_sin(ctx, NULL, t, LIBSAME_ATTN_SIG_FREQ_SECOND) /
      (int32_t)sizeof(int16_t);
#elif defined(LIBSAME_CONFIG_SINE_USE_LUT)
  const int32_t first_sample =
      libsame_sin(ctx, &ctx->sin_gen_lut.attn_sig_phase_first, t,
                  LIBSAME_ATTN_SIG_FREQ_FIRST) /
      (int32_t)sizeof(int16_t);
  const int32_t second_sample =
      libsame_sin(ctx, &ctx->sin_gen_lut.attn_sig_phase_second, t,
                  LIBSAME_ATTN_SIG_FREQ_SECOND) /
      (int32_t)sizeof(int16_t);
#elif defined(LIBSAME_CONFIG_SINE_USE_TAYLOR)
  ;
#else
#error "Unhandled generation engine for attention signal!"
#endif

  ctx->sample_data[sample_pos] = ((int16_t)(first_sample + second_sample));

  ctx->attn_sig_sample_num++;
}

void libsame_ctx_init(struct libsame_gen_ctx *const restrict ctx,
                      const struct libsame_header *const restrict header) {
  LIBSAME_ASSERT(ctx != NULL);
  LIBSAME_ASSERT(header != NULL);

  static const uint8_t LIBSAME_INITIAL_HEADER[] = {LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   LIBSAME_PREAMBLE,
                                                   'Z',
                                                   'C',
                                                   'Z',
                                                   'C',
                                                   '-',
                                                   'O',
                                                   'R',
                                                   'G',
                                                   '-',
                                                   'E',
                                                   'E',
                                                   'E',
                                                   '-',
                                                   'P',
                                                   'S',
                                                   'S',
                                                   'C',
                                                   'C',
                                                   'C'};

  memcpy(&ctx->header_data, LIBSAME_INITIAL_HEADER,
         sizeof(LIBSAME_INITIAL_HEADER));

  // We want to start populating the fields after the first dash.
  ctx->header_size = LIBSAME_PREAMBLE_NUM + LIBSAME_ASCII_ID_LEN + 1;

  libsame_field_add(ctx->header_data, &ctx->header_size,
                    header->originator_code, LIBSAME_ORIGINATOR_CODE_LEN);
  libsame_field_add(ctx->header_data, &ctx->header_size, header->event_code,
                    LIBSAME_EVENT_CODE_LEN);

  for (size_t i = 0; i < LIBSAME_LOCATION_CODES_NUM_MAX; ++i) {
    if (memcmp(header->location_codes[i], LIBSAME_LOCATION_CODE_END_MARKER,
               LIBSAME_LOCATION_CODE_LEN) == 0) {
      break;
    }
    libsame_field_add(ctx->header_data, &ctx->header_size,
                      header->location_codes[i], LIBSAME_LOCATION_CODE_LEN);
  }
  ctx->header_data[ctx->header_size - 1] = '+';

  libsame_field_add(ctx->header_data, &ctx->header_size,
                    header->valid_time_period, LIBSAME_VALID_TIME_PERIOD_LEN);

  libsame_field_add(ctx->header_data, &ctx->header_size,
                    header->originator_time, LIBSAME_ORIGINATOR_TIME_LEN);

  libsame_field_add(ctx->header_data, &ctx->header_size, header->callsign,
                    LIBSAME_CALLSIGN_LEN);

  // clang-format off
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_FIRST] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_SECOND] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_THIRD] =
  LIBSAME_AFSK_BITS_PER_CHAR * LIBSAME_AFSK_SAMPLES_PER_BIT *
  (unsigned int)ctx->header_size;

  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_FIRST] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_SECOND] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_THIRD] =
  LIBSAME_AFSK_BITS_PER_CHAR * LIBSAME_AFSK_SAMPLES_PER_BIT *
  LIBSAME_EOM_HEADER_SIZE;

  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FIRST] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SECOND] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_THIRD] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FOURTH] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FIFTH] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SIXTH] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SEVENTH] =
  LIBSAME_SILENCE_DURATION * LIBSAME_SAMPLE_RATE;

  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_ATTENTION_SIGNAL] =
  header->attn_sig_duration * LIBSAME_SAMPLE_RATE;
  // clang-format on

#ifdef LIBSAME_CONFIG_SINE_USE_LUT
  for (size_t sample_num = 0; sample_num < LIBSAME_CONFIG_SINE_LUT_SIZE;
       ++sample_num) {
    const float t = (float)sample_num / LIBSAME_CONFIG_SINE_LUT_SIZE;
    const float sine = sinf(LIBSAME_PI * 2 * t);

    const int16_t sample = (int16_t)(sine * INT16_MAX);
    ctx->sin_gen_lut.entries[sample_num] = sample;
  }
#endif  // LIBSAME_CONFIG_SINE_USE_LUT
}

void libsame_samples_gen(struct libsame_gen_ctx *const ctx) {
  LIBSAME_ASSERT(ctx != NULL);

  static const uint8_t LIBSAME_EOM_HEADER[LIBSAME_EOM_HEADER_SIZE] = {
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      LIBSAME_PREAMBLE,
      'N',
      'N',
      'N',
      'N'};

  // Tried to generate a SAME header using a context for which a SAME header was
  // already generated; bug.
  LIBSAME_ASSERT(ctx->seq_state < LIBSAME_SEQ_STATE_NUM);

  for (unsigned int sample_count = 0; sample_count < LIBSAME_SAMPLES_NUM_MAX;
       ++sample_count) {
    switch (ctx->seq_state) {
      case LIBSAME_SEQ_STATE_AFSK_HEADER_FIRST:
      case LIBSAME_SEQ_STATE_AFSK_HEADER_SECOND:
      case LIBSAME_SEQ_STATE_AFSK_HEADER_THIRD:
        libsame_afsk_gen(ctx, ctx->header_data, ctx->header_size, sample_count);
        break;

      case LIBSAME_SEQ_STATE_SILENCE_FIRST:
      case LIBSAME_SEQ_STATE_SILENCE_SECOND:
      case LIBSAME_SEQ_STATE_SILENCE_THIRD:
      case LIBSAME_SEQ_STATE_SILENCE_FOURTH:
      case LIBSAME_SEQ_STATE_SILENCE_FIFTH:
      case LIBSAME_SEQ_STATE_SILENCE_SIXTH:
      case LIBSAME_SEQ_STATE_SILENCE_SEVENTH:
        libsame_silence_gen(ctx, sample_count);
        break;

      case LIBSAME_SEQ_STATE_ATTENTION_SIGNAL:
        libsame_attn_sig_gen(ctx, sample_count);
        break;

      case LIBSAME_SEQ_STATE_AFSK_EOM_FIRST:
      case LIBSAME_SEQ_STATE_AFSK_EOM_SECOND:
      case LIBSAME_SEQ_STATE_AFSK_EOM_THIRD:
        libsame_afsk_gen(ctx, LIBSAME_EOM_HEADER, LIBSAME_EOM_HEADER_SIZE,
                         sample_count);
        break;

      default:
        LIBSAME_UNREACHABLE;
        break;
    }
    ctx->seq_samples_remaining[ctx->seq_state]--;

    if (ctx->seq_samples_remaining[ctx->seq_state] == 0) {
      ctx->seq_state++;

      if (ctx->seq_state >= LIBSAME_SEQ_STATE_NUM) {
        return;
      }
    }
  }
}
