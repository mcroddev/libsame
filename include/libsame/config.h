/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright 2023 Michael Rodriguez
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LIBSAME_CONFIG_H
#define LIBSAME_CONFIG_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * When generating a sine wave, use the standard sinf() function as provided by
 * the libc. This is the default.
 */
#define LIBSAME_CONFIG_SINE_USE_LIBC

/**
 * When generating a sine wave, use a pre-calculated lookup table with a phase
 * accumulator and linear interpolation.
 *
 * Faster on most embedded systems at the cost of increased code size.
 */
/* #define LIBSAME_CONFIG_SINE_USE_LUT */

/**
 * When generating a sine wave, use a low-order Taylor series approximation.
 * Faster on systems where the multiplications can be pipelined.
 */
/* #define LIBSAME_CONFIG_SINE_USE_TAYLOR */

/* When generating a sine wave, the application provides the generation. */
/* #define LIBSAME_CONFIG_SINE_USE_APP */

/**
 * If using the lookup table, how many sine wave samples should we generate?
 * Default is 1024.
 */
#if defined(LIBSAME_CONFIG_SINE_USE_LUT) && \
    !defined(LIBSAME_CONFIG_SINE_LUT_SIZE)
#define LIBSAME_CONFIG_SINE_LUT_SIZE (1024U)
#endif

#if defined(LIBSAME_CONFIG_SINE_USE_LIBC) +       \
        defined(LIBSAME_CONFIG_SINE_USE_TABLE) +  \
        defined(LIBSAME_CONFIG_SINE_USE_TAYLOR) + \
        defined(LIBSAME_CONFIG_SINE_USE_APP) !=   \
    1
#error \
    "You must define at least one, and only one of the following: LIBSAME_CONFIG_SINE_USE_LIBC, LIBSAME_CONFIG_SINE_USE_TABLE, LIBSAME_CONFIG_SINE_USE_TAYLOR, or LIBSAME_CONFIGs_SINE_USE_APP"
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* LIBSAME_CONFIG_H */

#ifdef __cplusplus
}
#endif /* __cplusplus */
