// SPDX-License-Identifier: MIT
//
// Copyright 2023-2024 Michael Rodriguez
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

#ifndef LIBSAME_PRIVATE_COMPILER_H
#define LIBSAME_PRIVATE_COMPILER_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#ifdef __GNUC__
/// Informs the compiler that this block will never be executed.
#define UNREACHABLE __builtin_unreachable()
#else
/// Informs the compiler that this block will never be executed.
#define UNREACHABLE
#endif  // __GNUC__

#ifdef __cplusplus
}
#endif  // __cplusplus

#endif  // LIBSAME_PRIVATE_COMPILER_H
