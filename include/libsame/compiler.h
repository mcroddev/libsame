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

/// @file compiler.h
/// Defines the compiler support structure for libsame.
///
/// Using a [GNU Compiler Collection](https://gcc.gnu.org) or
/// [Clang/LLVM](https://clang.llvm.org/) based compiler is recommended.

#ifndef LIBSAME_COMPILER_H
#define LIBSAME_COMPILER_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#if defined(__clang__) || defined(__GNUC__)
/// Informs the compiler that some code will never be executed.
#define LIBSAME_UNREACHABLE __builtin_unreachable()

/// Informs the compiler to always inline a function regardless of compiler
/// optimization level and/or even if the compiler heuristics do not evaluate it
/// to be beneficial.
#define LIBSAME_ALWAYS_INLINE inline __attribute__((always_inline))

/// Informs the compiler that a particular branch is unlikely to be executed.
#define LIBSAME_UNLIKELY(expr) __builtin_expect(!!(expr), 0)

/// Informs the compiler that a particular branch is likely to be executed.
#define LIBSAME_LIKELY(expr) __builtin_expect(!!(expr), 1)
#else
/// Informs the compiler that some code will never be executed.
#define LIBSAME_UNREACHABLE

/// Informs the compiler to always inline a function regardless of compiler
/// optimization level and/or even if the compiler heuristics do not evaluate it
/// to be beneficial.
#define LIBSAME_ALWAYS_INLINE

/// Informs the compiler that a particular branch is unlikely to be executed.
#define LIBSAME_UNLIKELY(expr)

/// Informs the compiler that a particular branch is likely to be executed.
#define LIBSAME_LIKELY(expr)
#endif  // defined(__clang__) || defined(__GNUC__)

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LIBSAME_COMPILER_H
