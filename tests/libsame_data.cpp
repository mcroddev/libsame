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

/** \file libsame_data.cpp
 *
 * These test cases verify that the data values being used are consistent with
 * the latest version of the EAS protocol as defined by 47 CFR 11.31, which is
 * located at https://preview.tinyurl.com/eas-ecfr.
 */

#include "gtest/gtest.h"
#include "libsame/libsame.h"

#ifndef NDEBUG
extern "C" void *libsame_userdata_ = nullptr;

extern "C" void libsame_assert_failed(const char *const, const char *const,
                                      const int, void *) {
  abort();
}
#endif  // NDEBUG

TEST(libsame_data, HeaderDataMaxSizeIsCorrect) {
  EXPECT_EQ(LIBSAME_HEADER_SIZE_MAX, 268);
}

/// Ensures that the sample rate is 44100Hz.
TEST(libsame_data, SampleRateIsCorrect) {
  EXPECT_EQ(LIBSAME_SAMPLE_RATE, 44100);
}

/// Ensures that the Preamble is 0xAB.
TEST(libsame_data, PreambleIsCorrect) { EXPECT_EQ(LIBSAME_PREAMBLE, 0xAB); }

/// Ensures that the number of times the Preamble must be transmitted is 16.
TEST(libsame_data, PreambleAppearsSixteenTimes) {
  EXPECT_EQ(LIBSAME_PREAMBLE_NUM, 16);
}

/// Ensures that the length of the originator code is 3.
TEST(libsame_data, OriginatorCodeLenIsCorrect) {
  EXPECT_EQ(LIBSAME_ORIGINATOR_CODE_LEN, 3);
}

/// Ensures that the number of characters for the event code is 3.
TEST(libsame_data, EventCodeMaxLenIsCorrect) {
  EXPECT_EQ(LIBSAME_EVENT_CODE_LEN, 3);
}

/// Ensures that the maximum number of location codes is 31.
TEST(libsame_data, LocationCodesMaxNumIsCorrect) {
  EXPECT_EQ(LIBSAME_LOCATION_CODES_NUM_MAX, 31);
}

/// Ensures that the number of characters for the location code is 6.
TEST(libsame_data, LocationCodeLenIsCorrect) {
  EXPECT_EQ(LIBSAME_LOCATION_CODE_LEN, 6);
}

/// Ensures that the number of characters for a valid time period is 4.
TEST(libsame_data, ValidTimePeriodLenIsCorrect) {
  EXPECT_EQ(LIBSAME_VALID_TIME_PERIOD_LEN, 4);
}

/// Ensures that the maximum number of characters for the originator time is 7.
TEST(libsame_data, OriginatorTimeLenIsCorrect) {
  EXPECT_EQ(LIBSAME_ORIGINATOR_TIME_LEN, 7);
}

/// Ensures that the maximum number of characters for the identification code is
/// 8.
TEST(libsame_data, CallsignLenIsCorrect) { EXPECT_EQ(LIBSAME_CALLSIGN_LEN, 8); }

/// Ensures that the End of Message (EOM) header size is correct.
TEST(libsame_data, EndOfMessageHeaderSizeIsCorrect) {
  EXPECT_EQ(LIBSAME_EOM_HEADER_SIZE, 20);
}

TEST(libsame_data, AFSKBitRateIsCorrect) {
  EXPECT_FLOAT_EQ(LIBSAME_AFSK_BIT_RATE, 520.83F);
}

TEST(libsame_data, AFSKMarkFreqIsCorrect) {
  EXPECT_FLOAT_EQ(LIBSAME_AFSK_MARK_FREQ, 2083.3F);
}

TEST(libsame_data, AFSKSpaceFreqIsCorrect) {
  EXPECT_FLOAT_EQ(LIBSAME_AFSK_SPACE_FREQ, 1562.5F);
}

TEST(libsame_data, AFSKBitDurationIsCorrect) {
  EXPECT_FLOAT_EQ(LIBSAME_AFSK_BIT_DURATION, 1.0F / LIBSAME_AFSK_BIT_RATE);
}

TEST(libsame_data, AFSKBitsPerCharIsCorrect) {
  EXPECT_EQ(LIBSAME_AFSK_BITS_PER_CHAR, 8);
}

TEST(libsame_data, AFSKSamplesPerBitIsCorrect) {
  EXPECT_EQ(LIBSAME_AFSK_SAMPLES_PER_BIT, 85);
}

TEST(libsame_data, AttnSigFreqFirstIsCorrect) {
  EXPECT_FLOAT_EQ(LIBSAME_ATTN_SIG_FREQ_FIRST, 853.0F);
}

TEST(libsame_data, AttnSigFreqSecondIsCorrect) {
  EXPECT_FLOAT_EQ(LIBSAME_ATTN_SIG_FREQ_SECOND, 960.0F);
}

TEST(libsame_data, AttnSigMinDurationIsCorrect) {
  EXPECT_EQ(LIBSAME_ATTN_SIG_DURATION_MIN, 8);
}

TEST(libsame_data, AttnSigMaxDurationIsCorrect) {
  EXPECT_EQ(LIBSAME_ATTN_SIG_DURATION_MAX, 25);
}
