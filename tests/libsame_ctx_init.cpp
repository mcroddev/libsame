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

#include "gtest/gtest.h"
#include "libsame/libsame.h"

namespace {
constexpr unsigned int SAMPLE_RATE = 44100;
constexpr unsigned int AFSK_BITS_PER_CHAR = 8;
constexpr unsigned int AFSK_SAMPLES_PER_BIT = 85;

struct libsame_gen_ctx ctx = {};

constexpr const struct libsame_header header = {
    .location_codes = {"101010", "010101", "SPOOKY"},
    .valid_time_period = "1000",
    .originator_code = "GOD",
    .event_code = "GOG",
    .callsign = "HEATISON",
    .originator_time = "1717777",
    .attn_sig_duration = 8};
};  // namespace

#ifndef NDEBUG
void *libsame_dbg_userdata_ = nullptr;

extern "C" [[noreturn]] void libsame_assert_failed(const char *const,
                                                   const char *const, const int,
                                                   void *) {
  std::abort();
}
#endif  // NDEBUG

TEST(libsame_ctx_init, SampleRateIsSet) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  EXPECT_EQ(ctx.sample_rate, SAMPLE_RATE);
}

TEST(libsame_ctx_init, AFSKSamplesPerBitCalculatesCorrectly) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  // We expect rounding to take place here; the real value without rounding here
  // is ~84.67254190426819 for SAMPLE_RATE, but casting this down to 84 causes a
  // loss of precision and thus causes us to fail to generate a good portion of
  // the message. So, make sure we round up.
  EXPECT_EQ(ctx.afsk_samples_per_bit, AFSK_SAMPLES_PER_BIT);
}

TEST(libsame_ctx_init, AFSKHeaderSamplesRemainingCalculatesCorrectly) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  static constexpr unsigned int EXPECTED_HEADER_SIZE = 65;
  static constexpr unsigned int AFSK_HEADER_TOTAL_SAMPLES =
      AFSK_BITS_PER_CHAR * AFSK_SAMPLES_PER_BIT * EXPECTED_HEADER_SIZE;

  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_FIRST],
            AFSK_HEADER_TOTAL_SAMPLES);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_SECOND],
            AFSK_HEADER_TOTAL_SAMPLES);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_HEADER_THIRD],
            AFSK_HEADER_TOTAL_SAMPLES);
}

TEST(libsame_ctx_init, AFSKEOMSamplesRemainingCalculatesCorrectly) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  static constexpr unsigned int EXPECTED_EOM_HEADER_SIZE = 20;
  static constexpr unsigned int AFSK_EOM_TOTAL_SAMPLES =
      AFSK_BITS_PER_CHAR * AFSK_SAMPLES_PER_BIT * EXPECTED_EOM_HEADER_SIZE;

  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_FIRST],
            AFSK_EOM_TOTAL_SAMPLES);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_SECOND],
            AFSK_EOM_TOTAL_SAMPLES);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_AFSK_EOM_THIRD],
            AFSK_EOM_TOTAL_SAMPLES);
}

TEST(libsame_ctx_init, SilenceSamplesRemainingCalculatesCorrectly) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  static constexpr unsigned int SILENCE_PERIOD = 1;

  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FIRST],
            SILENCE_PERIOD * SAMPLE_RATE);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SECOND],
            SILENCE_PERIOD * SAMPLE_RATE);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_THIRD],
            SILENCE_PERIOD * SAMPLE_RATE);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FOURTH],
            SILENCE_PERIOD * SAMPLE_RATE);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_FIFTH],
            SILENCE_PERIOD * SAMPLE_RATE);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SIXTH],
            SILENCE_PERIOD * SAMPLE_RATE);
  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_SILENCE_SEVENTH],
            SILENCE_PERIOD * SAMPLE_RATE);
}

TEST(libsame_ctx_init, AttentionSignalSamplesCalculatesCorrectly) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  static constexpr unsigned int NUM_SAMPLES =
      header.attn_sig_duration * SAMPLE_RATE;

  EXPECT_EQ(ctx.seq_samples_remaining[LIBSAME_SEQ_STATE_ATTENTION_SIGNAL],
            NUM_SAMPLES);
}

TEST(libsame_ctx_init, PreambleIsPresent) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  static constexpr size_t PREAMBLE_LEN = 16;
  static constexpr std::uint8_t PREAMBLE = 0xAB;

  for (size_t i = 0; i < PREAMBLE_LEN; ++i) {
    EXPECT_EQ(ctx.header_data[i], PREAMBLE);
  }
}

TEST(libsame_ctx_init, StartMarkerIsPresent) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  const std::string start_marker("ZCZC");
  const std::string str(reinterpret_cast<const char *>(&ctx.header_data[16]),
                        start_marker.length());

  EXPECT_EQ(start_marker, str);
}

TEST(libsame_ctx_init, FieldsAreAdded) {
  ctx = {};
  libsame_ctx_init(&ctx, &header, SAMPLE_RATE);

  const std::string expected_result(
      "GOD-GOG-101010-010101+1000-1717777-HEATISON-");

  // Start at the 21st position. This is the position after the preamble, start
  // of header code, and the initial dash.
  const std::string str(reinterpret_cast<const char *>(&ctx.header_data[21]),
                        expected_result.length());

  EXPECT_EQ(str, expected_result);
}
