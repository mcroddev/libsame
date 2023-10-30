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

#include <cstring>

#include "gtest/gtest.h"
#include "libsame/libsame.h"

#ifndef NDEBUG
extern "C" void *libsame_userdata_ = nullptr;

extern "C" [[noreturn]] void libsame_assert_failed(const char *const,
                                                   const char *const, const int,
                                                   void *) {
  std::abort();
}

TEST(libsame_ctx_init, AssertsWhenContextIsNULL) {
  struct libsame_header header = {};
  EXPECT_DEATH({ libsame_ctx_init(nullptr, &header); }, ".*");
}

TEST(libsame_ctx_init, AssertsWhenHeaderIsNULL) {
  struct libsame_gen_ctx ctx = {};
  EXPECT_DEATH({ libsame_ctx_init(&ctx, nullptr); }, ".*");
}
#endif  // NDEBUG

TEST(libsame_ctx_init, InitialHeaderIsCorrect) {
  static const std::uint8_t LIBSAME_INITIAL_HEADER[] = {LIBSAME_PREAMBLE,
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
                                                        'C'};

  struct libsame_gen_ctx ctx = {};
  struct libsame_header header = {};

  libsame_ctx_init(&ctx, &header);

  const int result = std::memcmp(&ctx.header_data, LIBSAME_INITIAL_HEADER,
                                 sizeof(LIBSAME_INITIAL_HEADER));

  EXPECT_EQ(result, 0);
}

TEST(libsame_ctx_init, OriginatorCodeAddedToHeaderData) {}

TEST(libsame_ctx_init, EventCodeAddedToHeaderData) {}
