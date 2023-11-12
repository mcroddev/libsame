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

#include <benchmark/benchmark.h>

#include <iostream>

#include "libsame/libsame.h"

static void benchmark_default_path(benchmark::State& state) {
  const struct libsame_header header = {
      .location_codes = {"048484", "048024", "048484", "048024", "048484",
                         "048024", "048484", "048024", "048484", "048024",
                         "048484", "048024", "048484", "048024", "048484",
                         "048024", "048484", "048024", "048484", "048024",
                         "048484", "048024", "048484", "048024", "048484",
                         "048024", "048484", "048024", "048484", "048024",
                         "048484"},
      .valid_time_period = "1000",
      .originator_code = "WXR",
      .event_code = "TOR",
      .callsign = "WAEB/AM ",
      .originator_time = "1172221",
      .attn_sig_duration = 8};

  struct libsame_gen_ctx ctx = {};
  std::cout << "Generation engine: " << libsame_gen_engine_desc_get() << '\n';

  for (auto _ : state) {
    libsame_init();
    libsame_ctx_init(&ctx, &header, 44100);

    while (ctx.seq_state != LIBSAME_SEQ_STATE_NUM) {
      libsame_samples_gen(&ctx);
    }
  }
}
BENCHMARK(benchmark_default_path);

BENCHMARK_MAIN();
