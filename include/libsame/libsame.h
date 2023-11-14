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
///
/// **WARNING:** Changing anything here may break ABI compatibility. The ABI
/// should under no circumstances break between versions unless there is a
/// protocol change or a serious bug at play.

#ifndef LIBSAME_LIBSAME_H
#define LIBSAME_LIBSAME_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stddef.h>
#include <stdint.h>

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

/// Defines the possible generation engines that are in use.
enum libsame_gen_engine {
  LIBSAME_GEN_ENGINE_LIBC,
  LIBSAME_GEN_ENGINE_LUT,
  LIBSAME_GEN_ENGINE_TAYLOR,
  LIBSAME_GEN_ENGINE_APP
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

  /// The header data to generate an AFSK burst from.
  uint8_t header_data[LIBSAME_HEADER_SIZE_MAX];

  /// The number of samples remaining for each generation sequence.
  unsigned int seq_samples_remaining[LIBSAME_SEQ_STATE_NUM];

  /// Defines the current AFSK state.
  struct {
    /// The current position within the data.
    size_t data_pos;

    /// The phase accumulator for AFSK bursts. This only matters if the
    /// generation engine is the LUT and is not intended for public use.
    float phase;

    /// The current bit we're generating a sine wave for.
    unsigned int bit_pos;

    /// The current sample we're generating.
    unsigned int sample_num;
  } afsk;

  /// The function to call when a sine wave needs to be generated.
  ///
  /// This only matters if the generation engine in use is the application
  /// specified generator.
  ///
  /// @param userdata Application specific userdata, if any.
  /// @param t The time period of the sine wave.
  /// @param freq The desired frequency of the sine wave.
  int16_t (*sin_gen)(void *const userdata, const float t, const float freq);

  /// Application specified userdata for the sine generation function, if any.
  ///
  /// This only matters if the generation engine in use is the application
  /// specified generator.
  void *sin_gen_userdata;

  /// The actual size of the header to care about.
  size_t header_size;

  /// The phase accumulator for the first fundamental frequency of the attention
  /// signal.
  float attn_sig_phase_first;

  /// The phase accumulator for the second fundamental frequency of the
  /// attention signal.
  float attn_sig_phase_second;

  /// The sample rate as specified by libsame_ctx_init().
  unsigned int sample_rate;

  /// The number of samples per bit as defined by the specified sample rate for
  /// AFSK bursts.
  unsigned int afsk_samples_per_bit;

  /// The current sequence of the generation.
  enum libsame_seq_state seq_state;

  /// The current sample we're generating for the attention signal.
  unsigned int attn_sig_sample_num;
};

void libsame_init(void);

void libsame_ctx_init(struct libsame_gen_ctx *const ctx,
                      const struct libsame_header *const header,
                      const unsigned int sample_rate);

void libsame_samples_gen(struct libsame_gen_ctx *const ctx);

enum libsame_gen_engine libsame_gen_engine_get(void);

const char *libsame_gen_engine_desc_get(void);

void libsame_attn_sig_durations_get(unsigned int *const min,
                                    unsigned int *const max);

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LIBSAME_LIBSAME_H
