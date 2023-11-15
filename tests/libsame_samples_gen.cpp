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

#ifndef NDEBUG
void *libsame_dbg_userdata_ = nullptr;

extern "C" [[noreturn]] void libsame_assert_failed(const char *const,
                                                   const char *const, const int,
                                                   void *) {
  std::abort();
}
#endif  // NDEBUG

/// Handles the overall logic for state transition testing.
class SeqStateTransitionTest : public ::testing::Test {
 protected:
  void SetUp() override { ctx = {}; }

  /// Verifies that a state transitions into another state.
  ///
  /// @param start_state The state to start with.
  /// @param expected_state The state we should transition to.
  void VerifyTransition(const enum libsame_seq_state start_state,
                        const enum libsame_seq_state expected_state) noexcept {
    ctx.seq_state = start_state;
    libsame_ctx_init(&ctx, &header, 44100);

    const unsigned int num_samples_expected =
        ctx.seq_samples_remaining[start_state];

    unsigned int count = 0;

    while (count < num_samples_expected) {
      libsame_samples_gen(&ctx);
      count += LIBSAME_SAMPLES_NUM_MAX;
    }
    EXPECT_EQ(ctx.seq_state, expected_state);
  }

 private:
  const struct libsame_header header = {
      .location_codes = {"101010", "828282", LIBSAME_LOCATION_CODE_END_MARKER},
      .valid_time_period = "2138",
      .originator_code = "ORG",
      .event_code = "RED",
      .callsign = "XIPHIAS ",
      .originator_time = "3939393",
      .attn_sig_duration = 8};

  struct libsame_gen_ctx ctx = {};
};

/// Verifies that the first AFSK header burst transitions to the first silence
/// period after the appropriate time.
TEST_F(SeqStateTransitionTest, FirstAFSKHeaderToFirstSilence) {
  VerifyTransition(LIBSAME_SEQ_STATE_AFSK_HEADER_FIRST,
                   LIBSAME_SEQ_STATE_SILENCE_FIRST);
}

/// Verifies that the second AFSK header burst transitions to the second silence
/// period after the appropriate time.
TEST_F(SeqStateTransitionTest, SecondAFSKHeaderToSecondSilence) {
  VerifyTransition(LIBSAME_SEQ_STATE_AFSK_HEADER_SECOND,
                   LIBSAME_SEQ_STATE_SILENCE_SECOND);
}

/// Verifies that the third AFSK header burst transitions to the third silence
/// period after the appropriate time.
TEST_F(SeqStateTransitionTest, ThirdAFSKHeaderToThirdSilence) {
  VerifyTransition(LIBSAME_SEQ_STATE_AFSK_HEADER_THIRD,
                   LIBSAME_SEQ_STATE_SILENCE_THIRD);
}

/// Verifies that the attention signal transitions to the fourth silence period
/// after the appropriate time.
TEST_F(SeqStateTransitionTest, AttentionSignalToFourthSilence) {
  VerifyTransition(LIBSAME_SEQ_STATE_ATTENTION_SIGNAL,
                   LIBSAME_SEQ_STATE_SILENCE_FOURTH);
}

/// Verifies that the first EOM burst transitions to the fifth silence period
/// after the appropriate time.
TEST_F(SeqStateTransitionTest, FirstEOMHeaderToFifthSilence) {
  VerifyTransition(LIBSAME_SEQ_STATE_AFSK_EOM_FIRST,
                   LIBSAME_SEQ_STATE_SILENCE_FIFTH);
}

/// Verifies that the second EOM burst transitions to the sixth silence period
/// after the appropriate time.
TEST_F(SeqStateTransitionTest, SecondEOMHeaderToSixthSilence) {
  VerifyTransition(LIBSAME_SEQ_STATE_AFSK_EOM_SECOND,
                   LIBSAME_SEQ_STATE_SILENCE_SIXTH);
}

/// Verifies that the third EOM burst transitions to the seventh silence period
/// after the appropriate time.
TEST_F(SeqStateTransitionTest, ThirdEOMHeaderToSeventhSilence) {
  VerifyTransition(LIBSAME_SEQ_STATE_AFSK_EOM_THIRD,
                   LIBSAME_SEQ_STATE_SILENCE_SEVENTH);
}
