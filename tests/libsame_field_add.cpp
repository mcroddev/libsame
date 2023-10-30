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

TEST(libsame_field_add, AssertsWhenDataIsNULL) {
  std::size_t x = 0;
  EXPECT_DEATH({ libsame_field_add(nullptr, &x, "TEST", 5); }, ".*");
}

TEST(libsame_field_add, AssertsWhenDataSizeIsNULL) {
  std::uint8_t x[10];
  EXPECT_DEATH({ libsame_field_add(x, nullptr, "TEST", 5); }, ".*");
}

TEST(libsame_field_add, AssertsWhenFieldIsNULL) {
  std::uint8_t x[10];
  std::size_t size;

  EXPECT_DEATH({ libsame_field_add(x, &size, nullptr, 5); }, ".*");
}

TEST(libsame_field_add, AssertsWhenFieldLenIsZero) {
  std::uint8_t x[10];
  std::size_t size;

  EXPECT_DEATH({ libsame_field_add(x, &size, "TEST", 0); }, ".*");
}

TEST(libsame_field_add, AssertsWhenFieldLenExceedsLargest) {
  std::uint8_t x[10];
  std::size_t size;

  EXPECT_DEATH({ libsame_field_add(x, &size, "TEST", 100); }, ".*");
}
#endif  // NDEBUG

TEST(libsame_field_add, AddsFieldToData) {
  std::size_t size = 0;
  std::uint8_t x[100] = {};

  libsame_field_add(x, &size, "FIELD", sizeof("FIELD") - 1);

  EXPECT_TRUE(std::memcmp(x, "FIELD-", sizeof("FIELD")) == 0);
  EXPECT_EQ(size, sizeof("FIELD"));  // Accounting for the dash!
}
