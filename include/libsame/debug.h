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

/// @file debug.h
/// Defines the debugging facilities of libsame.
///
/// These facilities only work if NDEBUG is not defined, otherwise they all
/// perform no operation and are optimized out.

#ifndef LIBSAME_DEBUG_H
#define LIBSAME_DEBUG_H

#pragma once

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

/// When unit tests are built, static functions become externally visible so
/// each one can also be directly tested. Functions which are meant to be
/// statically declared in a normal case should not use `static` directly, but
/// rather `LIBSAME_STATIC`.
#ifndef LIBSAME_TESTING
#define LIBSAME_STATIC static
#else
#define LIBSAME_STATIC
#endif  // LIBSAME_TESTING

#ifndef NDEBUG
/// Equivalent functionality to the assert() macro.
#define LIBSAME_ASSERT(expr)                                               \
  do {                                                                     \
    if (!LIBSAME_UNLIKELY((expr))) {                                       \
      libsame_assert_failed(#expr, __FILE__, __LINE__, libsame_userdata_); \
    }                                                                      \
  } while (0)

/// This function is called when an assertion is hit.
///
/// In the event of an assertion, instead of a straight abort() call it may be
/// desired to perform some additional action on the application side before
/// terminating.
///
/// Control **MUST NOT** be returned to the originator of an assertion!
///
/// @param expr The expression which failed.
/// @param file The file where the assertion occurred.
/// @param line_no The line number where the assertion was hit.
/// @param userdata Application specific user data, if any.
/// @note This function is meant to be implemented by the application developer,
///       where it will be resolved at link time. As such, a definition does not
///       exist in libsame.
extern void libsame_assert_failed(const char *const expr,
                                  const char *const file, const int line_no,
                                  void *userdata);

/// Application specific user data to pass to libsame_assert_failed() if an
/// assertion occurs.
///
/// @note This variable is meant to be implemented by the application developer,
///       where it will be resolved at link time. As such, a definition does not
///       exist in libsame.
extern void *libsame_userdata_;

#else
/// Equivalent functionality to the assert() macro.
#define LIBSAME_ASSERT(x)
#endif  // NDEBUG

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LIBSAME_DEBUG_H
