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

/// Verifies that the attention signal durations returned are valid according to
/// the current specification.
TEST(libsame_attn_sig_durations_get, ValidPerProtocolSpec) {
  unsigned int min, max;
  libsame_attn_sig_durations_get(&min, &max);

  EXPECT_EQ(min, 8);
  EXPECT_EQ(max, 25);
}
