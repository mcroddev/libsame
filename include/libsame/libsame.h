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

/// @file libsame.h
/// Defines the API of the library, along with various structures and constants
/// necessary to drive functionality. Generally speaking, the protocol specific
/// constants need not be changed unless the protocol has changed.

#ifndef LIBSAME_LIBSAME_H
#define LIBSAME_LIBSAME_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stddef.h>
#include <stdint.h>

#include "config.h"

/// Consecutive string of bits (sixteen bytes of AB hexadecimal [8 bit byte
/// 1010'1011]) sent to clear the system, set AGC and set asynchronous decoder
/// clocking cycles.
///
/// The preamble must be transmitted before each header and End of Message code.
#define LIBSAME_PREAMBLE (UINT8_C(0xAB))

/// The number of times the preamble will appear.
#define LIBSAME_PREAMBLE_NUM (16U)

/// The length of the identifier sent to indicate the start of ASCII code.
#define LIBSAME_ASCII_ID_LEN (4U)

/// The length of the originator code (ORG) field.
#define LIBSAME_ORIGINATOR_CODE_LEN (3U)

/// The length of the event code (EEE) field.
#define LIBSAME_EVENT_CODE_LEN (3U)

/// The maximum number of location codes we're allowed to transmit.
#define LIBSAME_LOCATION_CODES_NUM_MAX (31U)

/// The length of a location code (PSSCCC) field.
#define LIBSAME_LOCATION_CODE_LEN (6U)

/// The length of the valid time period (TTTT) field.
#define LIBSAME_VALID_TIME_PERIOD_LEN (4U)

/// The length of the originator time (JJJHHMM) field.
#define LIBSAME_ORIGINATOR_TIME_LEN (7U)

/// The length of the callsign (LLLLLLLL) field.
#define LIBSAME_CALLSIGN_LEN (8U)

/// The total number of fields which make up a message, not counting the
/// preamble or the ASCII start code.
#define LIBSAME_FIELDS_NUM_TOTAL (6U)

/// The number of bytes which compose the End of Message (EOM) transmission.
#define LIBSAME_EOM_HEADER_SIZE (LIBSAME_PREAMBLE_NUM + 4U)

/// The maximum number of characters a header can hold.
///
/// @note Do not adjust this macro directly; adjust the values it references
/// instead.
#define LIBSAME_HEADER_SIZE_MAX                                                \
  (LIBSAME_PREAMBLE_NUM + LIBSAME_ASCII_ID_LEN + LIBSAME_ORIGINATOR_CODE_LEN + \
   LIBSAME_EVENT_CODE_LEN +                                                    \
   (LIBSAME_LOCATION_CODES_NUM_MAX * LIBSAME_LOCATION_CODE_LEN) +              \
   LIBSAME_VALID_TIME_PERIOD_LEN + LIBSAME_ORIGINATOR_TIME_LEN +               \
   LIBSAME_CALLSIGN_LEN + LIBSAME_LOCATION_CODES_NUM_MAX +                     \
   LIBSAME_FIELDS_NUM_TOTAL)

/// This value should be copied to the location code index corresponding to the
/// final entry + 1.
///
/// Example: If you specify only two location codes, the location codes
/// specified in a libsame_core_header's location code array should be arranged
/// like so:
///
///     [0]: 010101
///     [1]: 101101
///     [2]: LIBSAME_LOCATION_CODE_END_MARKER
#define LIBSAME_LOCATION_CODE_END_MARKER ("SPOOKY")

/// The number of audio samples per chunk.
#define LIBSAME_SAMPLES_NUM_MAX (4096U)

/// The number of audio samples per second.
///
/// This value is not defined in the protocol specification; however, it is not
/// unreasonable to assume 44100 Hz. Testing with various decoders has not
/// revealed any issues.
#define LIBSAME_SAMPLE_RATE (44100U)

/// The length of a period of silence in seconds.
#define LIBSAME_SILENCE_DURATION (1U)

/// The first fundamental frequency of the attention signal.
#define LIBSAME_ATTN_SIG_FREQ_FIRST (853.0F)

/// The second fundamental frequency of the attention signal.
#define LIBSAME_ATTN_SIG_FREQ_SECOND (960.0F)

/// The Preamble and EAS codes must use Audio Frequency Shift Keying at a rate
/// of 520.83 bits per second to transmit the codes.
#define LIBSAME_AFSK_BIT_RATE (520.83F)

/// Mark and space time must be 1.92 milliseconds.
#define LIBSAME_AFSK_BIT_DURATION (1.0F / LIBSAME_AFSK_BIT_RATE)

/// Mark frequency is 2083.3Hz.
#define LIBSAME_AFSK_MARK_FREQ (2083.3F)

/// Space frequency is 1562.5Hz.
#define LIBSAME_AFSK_SPACE_FREQ (1562.5F)

/// How many samples should we generate for each bit during an AFSK burst?
///
/// **WARNING:** The result should always be rounded up!
#define LIBSAME_AFSK_SAMPLES_PER_BIT \
  ((unsigned int)((LIBSAME_AFSK_BIT_DURATION * LIBSAME_SAMPLE_RATE) + 0.5F))

/// How many bits per character?
#define LIBSAME_AFSK_BITS_PER_CHAR (8U)

/// The minimum number of seconds the attention signal can last for.
#define LIBSAME_ATTN_SIG_DURATION_MIN (8U)

/// The maximum number of seconds the attention signal can last for.
#define LIBSAME_ATTN_SIG_DURATION_MAX (25U)

/// Defines the generation sequence states.
///
/// The sequence states dictate what portion of the SAME header we are
/// generating. These states are laid out in the natural order as one would
/// hear.
enum libsame_seq_state {
  /// First AFSK burst of header
  LIBSAME_SEQ_STATE_AFSK_HEADER_FIRST,

  /// 1 period of silence
  LIBSAME_SEQ_STATE_SILENCE_FIRST,

  /// Second AFSK burst of header
  LIBSAME_SEQ_STATE_AFSK_HEADER_SECOND,

  /// 1 period of silence
  LIBSAME_SEQ_STATE_SILENCE_SECOND,

  /// Third AFSK burst of header
  LIBSAME_SEQ_STATE_AFSK_HEADER_THIRD,

  /// 1 period of silence
  LIBSAME_SEQ_STATE_SILENCE_THIRD,

  /// Attention signal for 8..25 seconds
  LIBSAME_SEQ_STATE_ATTENTION_SIGNAL,

  /// 1 period of silence
  LIBSAME_SEQ_STATE_SILENCE_FOURTH,

  /// First AFSK burst of EOM
  LIBSAME_SEQ_STATE_AFSK_EOM_FIRST,

  /// 1 period of silence
  LIBSAME_SEQ_STATE_SILENCE_FIFTH,

  /// 1 second AFSK burst of EOM
  LIBSAME_SEQ_STATE_AFSK_EOM_SECOND,

  /// 1 period of silence
  LIBSAME_SEQ_STATE_SILENCE_SIXTH,

  /// Third AFSK burst of EOM
  LIBSAME_SEQ_STATE_AFSK_EOM_THIRD,

  /// 1 period of silence
  LIBSAME_SEQ_STATE_SILENCE_SEVENTH,

  /// The total number of sequence states. Do not modify or remove this entry.
  LIBSAME_SEQ_STATE_NUM
};

/// Defines the header to be used for transmission.
struct libsame_header {
  /// Indicates the geographic areas affected by the EAS alert.
  char location_codes[LIBSAME_LOCATION_CODES_NUM_MAX]
                     [LIBSAME_LOCATION_CODE_LEN + 1];

  /// Indicates the valid time period of a message.
  char valid_time_period[LIBSAME_VALID_TIME_PERIOD_LEN + 1];

  /// Indicates who originally initiated the activation of the EAS.
  char originator_code[LIBSAME_ORIGINATOR_CODE_LEN + 1];

  /// Indicates the nature of the EAS activation.
  char event_code[LIBSAME_EVENT_CODE_LEN + 1];

  /// Identification of the EAS Participant, NWS office, etc., transmitting or
  /// retransmitting the message. These codes will automatically be affixed to
  /// all outgoing messages by the EAS encoder.
  char callsign[LIBSAME_CALLSIGN_LEN + 1];

  /// Indicates when the message was initially released by the originator.
  char originator_time[LIBSAME_ORIGINATOR_TIME_LEN + 1];

  /// How long will the attention signal last for?
  unsigned int attn_sig_duration;
};

/// Defines the generation context.
///
/// A generation context keeps track of the audio generation state over each
/// call to the libsame_samples_gen function.
struct libsame_gen_ctx {
  /// The buffer containing the audio samples.
  int16_t sample_data[LIBSAME_SAMPLES_NUM_MAX];

#ifdef LIBSAME_CONFIG_SINE_USE_LUT
  /// Defines the sine wave lookup table data.
  struct {
    /// The lookup table containing the generated sine wave samples.
    int16_t entries[LIBSAME_CONFIG_SINE_LUT_SIZE];

    /// The phase accumulator for the first fundamental frequency of the
    /// attention signal.
    float attn_sig_phase_first;

    /// The phase accumulator for the second fundamental frequency of the
    /// attention signal.
    float attn_sig_phase_second;
  } sin_gen_lut;
#endif  // LIBSAME_CONFIG_SINE_USE_LUT

#ifdef LIBSAME_CONFIG_SINE_USE_APP
  /// The function to call when a sine wave needs to be generated.
  ///
  /// @param userdata Application specific userdata, if any.
  /// @param The time period of the sine wave.
  /// @param The desired frequency of the sine wave.
  int16_t (*sin_gen)(void *const userdata, const float t, const float freq);

  /// Application specified userdata for the sine generation function, if any.
  void *sin_gen_userdata;
#endif

  /// The header data to generate an AFSK burst from.
  uint8_t header_data[LIBSAME_HEADER_SIZE_MAX];

  /// The number of samples remaining for each generation sequence.
  unsigned int seq_samples_remaining[LIBSAME_SEQ_STATE_NUM];

  /// Defines the current AFSK state.
  struct {
    /// The current position within the data.
    size_t data_pos;

#ifdef LIBSAME_CONFIG_SINE_USE_LUT
    /// The phase accumulator for AFSK bursts.
    float phase;
#endif  // LIBSAME_CONFIG_SINE_USE_LUT

    /// The current bit we're generating a sine wave for.
    unsigned int bit_pos;

    /// The current sample we're generating.
    unsigned int sample_num;
  } afsk;

  /// The actual size of the header to care about.
  size_t header_size;

  /// The current sequence of the generation.
  enum libsame_seq_state seq_state;

  /// The current sample we're generating for the attention signal.
  unsigned int attn_sig_sample_num;
};

#ifdef LIBSAME_TESTING
int16_t libsame_sin(struct libsame_gen_ctx *const ctx, float *const phase,
                    const float t, const float freq);

void libsame_field_add(uint8_t *const data, size_t *data_size,
                       const char *const field, const size_t field_len);

void libsame_afsk_gen(struct libsame_gen_ctx *const ctx,
                      const uint8_t *const data, const size_t data_size,
                      const size_t sample_pos);

void libsame_silence_gen(struct libsame_gen_ctx *const ctx,
                         const size_t sample_pos);

void libsame_attn_sig_gen(struct libsame_gen_ctx *const ctx,
                          const size_t sample_pos);
#endif  // LIBSAME_TESTING

void libsame_ctx_init(struct libsame_gen_ctx *const ctx,
                      const struct libsame_header *const header);

void libsame_samples_gen(struct libsame_gen_ctx *const ctx);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LIBSAME_LIBSAME_H
