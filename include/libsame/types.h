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

/// @file types.h
///
/// This header file merely typedefs the existing fixed-width integer types and
/// some built-in types to shorter names for brevity and heightened clarity.

#ifndef LIBSAME_PUBLIC_TYPES_H
#define LIBSAME_PUBLIC_TYPES_H

#pragma once

#include <stdint.h>

/// @brief Unsigned integer.
typedef unsigned int uint;

/// @brief Unsigned 64-bit integer.
typedef uint64_t u64;

/// @brief Signed 64-bit integer.
typedef int64_t s64;

/// @brief Unsigned 32-bit integer.
typedef uint32_t u32;

/// @brief Signed 32-bit integer.
typedef int32_t s32;

/// @brief Unsigned 16-bit integer.
typedef uint16_t u16;

/// @brief Signed 16-bit integer.
typedef int16_t s16;

/// @brief Unsigned 8-bit integer.
typedef uint8_t u8;

/// @brief Signed 8-bit integer.
typedef int8_t s8;

#endif  // LIBSAME_PUBLIC_TYPES_H
