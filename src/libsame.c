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
/// * Single-precision floating point is enforced as double-precision is not
///   necessary.
///
/// * Dynamic memory allocation is forbidden; all sizes are fixed to reasonable
///   upper bounds.
///
/// The maximum number of samples that can be generated is defined by the
/// following:
///
///     - We arbitrarily define the sample rate to be 44100 Hz, since an
///       authoritative answer is not defined in the specification. Experiments
///       with various decoders has not revealed any issues.
///
///     - There are 7 periods of silence, each lasting 1 second: 1 after each
///       message transmission (there are 3), 1 after the attention signal, and
///       1 after each End of Message (EOM) transmission (there are 3).
///
///     - There are a maximum of 268 bytes that can be transmitted during an
///       AFSK burst, which occurs three times.
///
///     - The End of Message (EOM) transmission is 20 bytes, which occurs three
///       times.
///
///     - There are a total of 6 burst transmissions; 3 for the message portion,
///       and 3 for the EOM portion.
///
///     - The maximum amount of time an attention signal can last for is 25
///       seconds.
///
///     - The duration of each bit is 1.92ms, and we must produce 520.83 bits
///       per second. This gives us a calculation of ((1.0 / 520.83) * 44100)
///       which gives us a sum of 84.672539. However, the value needs to be
///       rounded UP to 85.
///
///     - There are 8 bits in a character.
///
/// The maximum number of samples can therefore be derived from the following
/// series of calculations (assuming a desired sample rate of 44100Hz):
///
///     8 bits/char * 85 samples/bit * 268 bytes * 3 bursts = 514,080
///     7 seconds * 44100Hz                                 = 308,700
///     25 seconds * 44100Hz                                = 1,102,500
///     8 bits/char * 85 samples/bit * 20 bytes * 3 bursts  = 40,800
///
///     514,080 + 308,700 + 1,102,500 + 40,800 = 1,966,080 samples
///
/// A 16-bit integer type is used for each sample, therefore the amount of space
/// required is (2 bytes * 1,966,080 samples), which equals
/// **3,932,160 bytes, or ~4 MB.**
///
/// Since it is not practical to use such a large value on the stack and dynamic
/// memory allocation is forbidden, we generate chunks of audio samples and then
/// push them to the audio device incrementally. We define 1 chunk to be 4,096
/// samples.

#include "libsame/libsame.h"

#include <math.h>
#include <string.h>

#include "libsame/compiler.h"
#include "libsame/debug.h"
#include "libsame_config.h"

/// The value of PI up to 35 decimal places.
#define LIBSAME_PI (3.14159265358979323846264338327950288F)

#ifdef LIBSAME_CONFIG_SINE_USE_LUT
static int16_t sin_lut[LIBSAME_CONFIG_SINE_LUT_SIZE];
#endif  // LIBSAME_CONFIG_SINE_USE_LUT

/// The expected size of the EOM header.
static const unsigned int EOM_HEADER_SIZE = LIBSAME_PREAMBLE_NUM + 4;

/// This is a consecutive string of bits (sixteen bytes of AB hexadecimal
/// [8 bit byte 10101011]) sent to clear the system, set AGC and set
/// asynchronous decoder clocking cycles. The preamble must be transmitted
/// before each header and End of Message code.
static const uint8_t PREAMBLE = 0xAB;

/// The Preamble and EAS Codes must use Audio Frequency Shift Keying at a rate
/// of 520.83 bits per second to transmit the codes.
static const float AFSK_BIT_RATE = 520.83F;

// Mark frequency is 2083.3 Hz.
static const float AFSK_MARK_FREQ = 2083.3F;

/// Space frequency is 1562.5 Hz.
static const float AFSK_SPACE_FREQ = 1562.5F;

/// The number of bits in a character.
static const unsigned int AFSK_BITS_PER_CHAR = 8;

/// Mark and space time must be 1.92 milliseconds.
static const float AFSK_BIT_DURATION = 1.0F / AFSK_BIT_RATE;

/// The first fundamental frequency of the attention signal.
static const float ATTN_SIG_FREQ_FIRST = 853.0F;

/// The second fundamental frequency of the attention signal.
static const float ATTN_SIG_FREQ_SECOND = 960.0F;

/// The number of seconds one period of silence should be.
static const unsigned int SILENCE_DURATION = 1;

/// The minimum duration of the attention signal in seconds.
static const unsigned int ATTN_SIG_DURATION_MIN = 8;

/// The maximum duration of the attention signal in seconds.
static const unsigned int ATTN_SIG_DURATION_MAX = 25;

/// Generates one sample of a sine wave.
///
/// This function is a wrapper around the possible generation engines that may
/// be used.
///
/// @param ctx The generation context in use.
/// @param phase The phase accumulator for the generation. This can be NULL if
///              the generation engine in use is not the LUT.
/// @param t The time period of the sine wave.
/// @param freq The desired frequency of the sine wave.
/// @returns The generated sine wave sample multiplied by INT16_MAX.
static int16_t sin_gen(struct libsame_gen_ctx *const restrict ctx,
                       float *const restrict phase, const float t,
                       const float freq) {
#if defined(LIBSAME_CONFIG_SINE_USE_LIBC)
  (void)ctx;
  (void)phase;
  return (int16_t)(sinf(LIBSAME_PI * 2 * t * freq) * INT16_MAX);
#elif defined(LIBSAME_CONFIG_SINE_USE_LUT)
  (void)t;
  LIBSAME_ASSERT(phase != NULL);

  const int16_t sample = sin_lut[(size_t)*phase];

  const float delta =
      (freq * LIBSAME_CONFIG_SINE_LUT_SIZE) / (float)ctx->sample_rate;

  *phase += delta;

  while (*phase >= LIBSAME_CONFIG_SINE_LUT_SIZE) {
    *phase -= LIBSAME_CONFIG_SINE_LUT_SIZE;
  }
  return sample;
#elif defined(LIBSAME_CONFIG_SINE_USE_TAYLOR)
  (void)ctx;
  (void)phase;

  float x = LIBSAME_PI * 2 * t * freq;

  unsigned int neg = x < 0.0F;
  if (neg) {
    x = -x;
  }
  x = fmodf(x, 2 * LIBSAME_PI);

  if (x >= LIBSAME_PI) {
    neg = !neg;
    x -= LIBSAME_PI;
  }

  // These factorials are precalculated for the low-ordered Taylor Series.
  const float FACT_T0 = 6.0F;     // 3
  const float FACT_T1 = 120.0F;   // 5
  const float FACT_T2 = 5040.0F;  // 7

  const float t0 = powf(x, 3) / FACT_T0;
  const float t1 = powf(x, 5) / FACT_T1;
  const float t2 = powf(x, 7) / FACT_T2;

  const float sample = (x - t0) + (t1 - t2);
  return (int16_t)((neg ? -sample : sample) * INT16_MAX);
#elif defined(LIBSAME_CONFIG_SINE_USE_APP)
  (void)phase;
  return ctx->sin_gen(&ctx->sin_gen_userdata, t, freq);
#else
#error "Unknown AFSK generation engine; implement it."
#endif
}

/// Adds a field to the header data.
///
/// A field is defined as any portion of the SAME header which must be populated
/// (e.g., originator code, event code).
///
/// @param data The data to append the field to.
/// @param data_size The new occupied space of the data.
/// @param field The field to append.
/// @param field_len The length of the field to append.
static void field_add(uint8_t *const restrict data, size_t *restrict data_size,
                      const char *restrict const field,
                      const size_t field_len) {
  LIBSAME_ASSERT(data != NULL);
  LIBSAME_ASSERT(data_size != NULL);
  LIBSAME_ASSERT(field != NULL);
  LIBSAME_ASSERT(field_len > 0);

  // XXX: This should always be set to the largest field!
  LIBSAME_ASSERT(field_len <= LIBSAME_CALLSIGN_LEN);

  memcpy(&data[*data_size], field, field_len);
  *data_size += field_len;
  data[(*data_size)++] = '-';
}

/// Generates an Audio Frequency Shift Keying (AFSK) burst.
///
/// @param ctx The generation context.
/// @param data The data to generate an AFSK burst from.
/// @param data_size The size of the data to generate an AFSK burst from.
/// @param sample_pos The position in the sample buffer to store the sample.
static void afsk_gen(struct libsame_gen_ctx *const restrict ctx,
                     const uint8_t *const restrict data, const size_t data_size,
                     const size_t sample_pos) {
  LIBSAME_ASSERT(ctx != NULL);
  LIBSAME_ASSERT(data != NULL);
  LIBSAME_ASSERT(data_size > 0);

  const float freq = ((data[ctx->afsk.data_pos] >> ctx->afsk.bit_pos) & 1)
                         ? AFSK_MARK_FREQ
                         : AFSK_SPACE_FREQ;

  const float t = (float)ctx->afsk.sample_num / (float)ctx->sample_rate;

  const int16_t sample = sin_gen(ctx, &ctx->afsk.phase, t, freq);

  ctx->sample_data[sample_pos] = sample;

  ctx->afsk.sample_num++;

  if (ctx->afsk.sample_num >= ctx->afsk_samples_per_bit) {
    ctx->afsk.sample_num = 0;
    ctx->afsk.bit_pos++;

    if (ctx->afsk.bit_pos >= AFSK_BITS_PER_CHAR) {
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

/// Generates a sample of silence.
///
/// To configure the length of silence, adjust LIBSAME_SILENCE_DURATION to an
/// appropriate value.
///
/// @param ctx The generation context to use.
/// @param sample_pos The position in the sample buffer to store the sample.
static void silence_gen(struct libsame_gen_ctx *const restrict ctx,
                        const size_t sample_pos) {
  LIBSAME_ASSERT(ctx != NULL);
  ctx->sample_data[sample_pos] = 0;
}

/// Generates the attention signal.
///
/// @param ctx The generation context to use.
/// @param sample_pos The position in the sample buffer to store the sample.
static void attn_sig_gen(struct libsame_gen_ctx *const restrict ctx,
                         const size_t sample_pos) {
  LIBSAME_ASSERT(ctx != NULL);

  const float t = (float)ctx->attn_sig_sample_num / (float)ctx->sample_rate;

  const int32_t first_sample =
      sin_gen(ctx, &ctx->attn_sig_phase_first, t, ATTN_SIG_FREQ_FIRST) /
      (int32_t)sizeof(int16_t);

  const int32_t second_sample =
      sin_gen(ctx, &ctx->attn_sig_phase_second, t, ATTN_SIG_FREQ_SECOND) /
      (int32_t)sizeof(int16_t);

  const int16_t sample = (int16_t)(first_sample + second_sample);
  ctx->sample_data[sample_pos] = sample;

  ctx->attn_sig_sample_num++;
}

/// Initializes libsame for use. This must be called before any context is
/// created and used.
void libsame_init(void) {
#ifdef LIBSAME_CONFIG_SINE_USE_LUT
  for (size_t sample_num = 0; sample_num < LIBSAME_CONFIG_SINE_LUT_SIZE;
       ++sample_num) {
    const float t = (float)sample_num / LIBSAME_CONFIG_SINE_LUT_SIZE;
    const float sine = sinf(LIBSAME_PI * 2 * t);

    const int16_t sample = (int16_t)(sine * INT16_MAX);
    sin_lut[sample_num] = sample;
  }
#endif  // LIBSAME_CONFIG_SINE_USE_LUT
}

/// Configures a generation context to generate the specified header.
///
/// This function determines how many samples are required to fully generate
/// each step of the header and translates the header structure into the string
/// that must be transmitted.
///
/// @param ctx The generation context.
/// @param header The header data to generate a SAME header from.
/// @param sample_rate The desired sample rate.
void libsame_ctx_init(struct libsame_gen_ctx *const restrict ctx,
                      const struct libsame_header *const restrict header,
                      const unsigned int sample_rate) {
  LIBSAME_ASSERT(ctx != NULL);
  LIBSAME_ASSERT(header != NULL);

  static const uint8_t LIBSAME_INITIAL_HEADER[] = {
      PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE,
      PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE,
      PREAMBLE, PREAMBLE, 'Z',      'C',      'Z',      'C',      '-',
      'O',      'R',      'G',      '-',      'E',      'E',      'E',
      '-',      'P',      'S',      'S',      'C',      'C',      'C'};

  memcpy(&ctx->header_data, LIBSAME_INITIAL_HEADER,
         sizeof(LIBSAME_INITIAL_HEADER));

  ctx->sample_rate = sample_rate;

  // We want to start populating the fields after the first dash.
  ctx->header_size = LIBSAME_PREAMBLE_NUM + LIBSAME_ASCII_ID_LEN + 1;

  field_add(ctx->header_data, &ctx->header_size, header->originator_code,
            LIBSAME_ORIGINATOR_CODE_LEN);
  field_add(ctx->header_data, &ctx->header_size, header->event_code,
            LIBSAME_EVENT_CODE_LEN);

  for (size_t i = 0; i < LIBSAME_LOCATION_CODES_NUM_MAX; ++i) {
    if (memcmp(header->location_codes[i], LIBSAME_LOCATION_CODE_END_MARKER,
               LIBSAME_LOCATION_CODE_LEN) == 0) {
      break;
    }
    field_add(ctx->header_data, &ctx->header_size, header->location_codes[i],
              LIBSAME_LOCATION_CODE_LEN);
  }
  ctx->header_data[ctx->header_size - 1] = '+';

  field_add(ctx->header_data, &ctx->header_size, header->valid_time_period,
            LIBSAME_VALID_TIME_PERIOD_LEN);

  field_add(ctx->header_data, &ctx->header_size, header->originator_time,
            LIBSAME_ORIGINATOR_TIME_LEN);

  field_add(ctx->header_data, &ctx->header_size, header->callsign,
            LIBSAME_CALLSIGN_LEN);

  ctx->afsk_samples_per_bit =
      (unsigned int)roundf(AFSK_BIT_DURATION * (float)ctx->sample_rate);

  // clang-format off
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_FIRST] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_SECOND] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_THIRD] =
  AFSK_BITS_PER_CHAR * ctx->afsk_samples_per_bit *
  (unsigned int)ctx->header_size;

  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_FIRST] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_SECOND] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_THIRD] =
  AFSK_BITS_PER_CHAR * ctx->afsk_samples_per_bit *
  EOM_HEADER_SIZE;

  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FIRST] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SECOND] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_THIRD] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FOURTH] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FIFTH] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SIXTH] =
  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SEVENTH] =
  SILENCE_DURATION * ctx->sample_rate;

  ctx->seq_samples_remaining[LIBSAME_SEQ_STATE_ATTENTION_SIGNAL] =
  header->attn_sig_duration * ctx->sample_rate;
  // clang-format on
}

/// Generates the audio samples for the SAME header using the specified
/// generation context.
///
/// The generation context *MUST* have been initialized by using
/// libsame_ctx_init() before calling this function.
///
/// @param ctx The generation context.
void libsame_samples_gen(struct libsame_gen_ctx *const restrict ctx) {
  LIBSAME_ASSERT(ctx != NULL);

  static const uint8_t EOM_HEADER[] = {
      PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE,
      PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE, PREAMBLE,
      PREAMBLE, PREAMBLE, 'N',      'N',      'N',      'N'};

  // Tried to generate a SAME header using a context for which a SAME header was
  // already generated; bug.
  LIBSAME_ASSERT(ctx->seq_state < LIBSAME_SEQ_STATE_NUM);

  for (unsigned int sample_count = 0; sample_count < LIBSAME_SAMPLES_NUM_MAX;
       ++sample_count) {
    switch (ctx->seq_state) {
      case LIBSAME_SEQ_STATE_AFSK_HEADER_FIRST:
      case LIBSAME_SEQ_STATE_AFSK_HEADER_SECOND:
      case LIBSAME_SEQ_STATE_AFSK_HEADER_THIRD:
        afsk_gen(ctx, ctx->header_data, ctx->header_size, sample_count);
        break;

      case LIBSAME_SEQ_STATE_SILENCE_FIRST:
      case LIBSAME_SEQ_STATE_SILENCE_SECOND:
      case LIBSAME_SEQ_STATE_SILENCE_THIRD:
      case LIBSAME_SEQ_STATE_SILENCE_FOURTH:
      case LIBSAME_SEQ_STATE_SILENCE_FIFTH:
      case LIBSAME_SEQ_STATE_SILENCE_SIXTH:
      case LIBSAME_SEQ_STATE_SILENCE_SEVENTH:
        silence_gen(ctx, sample_count);
        break;

      case LIBSAME_SEQ_STATE_ATTENTION_SIGNAL:
        attn_sig_gen(ctx, sample_count);
        break;

      case LIBSAME_SEQ_STATE_AFSK_EOM_FIRST:
      case LIBSAME_SEQ_STATE_AFSK_EOM_SECOND:
      case LIBSAME_SEQ_STATE_AFSK_EOM_THIRD:
        afsk_gen(ctx, EOM_HEADER, EOM_HEADER_SIZE, sample_count);
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

/// Retrieves the generation engine this version of libsame was compiled for.
///
/// @returns The generation engine this version of libsame was compiled for.
enum libsame_gen_engine libsame_gen_engine_get(void) {
#if defined(LIBSAME_CONFIG_SINE_USE_LIBC)
  return LIBSAME_GEN_ENGINE_LIBC;
#elif defined(LIBSAME_CONFIG_SINE_USE_LUT)
  return LIBSAME_GEN_ENGINE_LUT;
#elif defined(LIBSAME_CONFIG_SINE_USE_TAYLOR)
  return LIBSAME_GEN_ENGINE_TAYLOR;
#elif defined(LIBSAME_CONFIG_SINE_USE_APP)
  return LIBSAME_GEN_ENGINE_APP;
#else
#error "Unknown generation engine!"
#endif
}

/// Retrieves the full description of the generation engine in use.
///
/// @returns The full description of the generation engine in use.
const char *libsame_gen_engine_desc_get(void) {
#if defined(LIBSAME_CONFIG_SINE_USE_LIBC)
  return "libc sinf()";
#elif defined(LIBSAME_CONFIG_SINE_USE_LUT)
  return "Sine wave lookup table using linear interpolation and phase "
         "accumulators";
#elif defined(LIBSAME_CONFIG_SINE_USE_TAYLOR)
  return "Three-order Taylor Series";
#elif defined(LIBSAME_CONFIG_SINE_USE_APP)
  return "Application specified generator";
#else
#error "Unknown generation engine!"
#endif
}

/// Retrieves both the minimum and maximum number of seconds an attention signal
/// can be.
///
/// An example usage for this function is to set the minimum/maximum values on a
/// GUI spinbox governing this attribute.
///
/// @param min Where to store the minimum duration of the attention signal.
/// @param max Where to store the maximum duration of the attention signal.
void libsame_attn_sig_durations_get(unsigned int *const restrict min,
                                    unsigned int *const restrict max) {
  LIBSAME_ASSERT(min != NULL);
  LIBSAME_ASSERT(max != NULL);

  *min = ATTN_SIG_DURATION_MIN;
  *max = ATTN_SIG_DURATION_MAX;
}
