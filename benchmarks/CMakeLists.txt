# SPDX-License-Identifier: MIT
#
# Copyright 2023 Michael Rodriguez
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the “Software”), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

find_package(benchmark QUIET)

if (NOT ${benchmark_FOUND})
  if (NOT LIBSAME_DOWNLOAD_MISSING_DEPS)
    message(FATAL_ERROR
            "benchmark was not found, and downloading missing dependencies is "
            "disabled.")
  else()
    include(FetchContent)

    FetchContent_Declare(
      benchmark
      GIT_REPOSITORY https://github.com/google/benchmark.git
      GIT_TAG v1.8.3
    )

    # Download any dependencies needed when building benchmark.
    set(BENCHMARK_DOWNLOAD_DEPENDENCIES ON CACHE BOOL "" FORCE)

    # Do not build the unit tests.
    set(BENCHMARK_ENABLE_GTEST_TESTS OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(benchmark)
  endif()
endif()

if (LIBSAME_BUILD_BENCHMARKS_WITH_SHARED_LIBRARY)
  if (NOT LIBSAME_BUILD_SHARED_LIBRARY)
    message(FATAL_ERROR
            "libsame: The benchmarks were asked to be linked with the shared "
            "library form of libsame, but the shared library form of libsame "
            "wasn't asked to be compiled. Pass "
            "-DLIBSAME_BUILD_SHARED_LIBRARY:BOOL=ON to your CMake invocation.")
  else()
    set(SAME_LIB sharedsame)
  endif()
else()
  if (NOT LIBSAME_BUILD_STATIC_LIBRARY)
    message(FATAL_ERROR
            "libsame: The benchmarks were asked to be linked with the static "
            "library form of libsame, but the static library form of libsame "
            "wasn't asked to be compiled. Pass "
            "-DLIBSAME_BUILD_STATIC_LIBRARY:BOOL=ON to your CMake invocation.")
  else()
    set(SAME_LIB staticsame)
  endif()
endif()

function(benchmark_add NAME SRCS)
  add_executable(${NAME} ${SRCS})

  target_link_libraries(${NAME} PRIVATE
                        ${SAME_LIB}
                        libsame-build-settings-cpp
                        benchmark::benchmark)
endfunction()

benchmark_add(libsame_benchmark_default default.cpp)
