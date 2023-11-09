# libsame

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [![Build libsame](https://github.com/mcroddev/libsame/actions/workflows/build.yml/badge.svg)](https://github.com/mcroddev/libsame/actions/workflows/build.yml)

libsame is a [Specific Area Message Encoding (SAME)](https://en.wikipedia.org/wiki/Specific_Area_Message_Encoding) header generator
written in the [C programming language](https://en.wikipedia.org/wiki/C_(programming_language)) targeting the [C17 standard](https://en.wikipedia.org/wiki/C17_(C_standard_revision)).

**WARNING**: The author is not responsible for the misuse of this software. The
output of this software has the potential to activate EAS decoder systems in the
continental United States of America and its territories, along with various
offshore marine areas.

Do **not**:

- transmit the output of this software over any radio communication unless
  so authorized.
- play the output of this software in public areas.
- use this software where life and limb are in jeopardy

If you are under U.S. jurisdiction, please read 
[Misuse of the Emergency Alert System (EAS) Sound](https://www.fcc.gov/enforcement/areas/misuse-eas-sound) for more information.

## Features

* Multiple generation engines
  - [C standard library](https://en.wikipedia.org/wiki/C_standard_library) `sinf()` function. This is the default.
  - Three-order [Taylor series](https://en.wikipedia.org/wiki/Taylor_series)
  - Sine wave lookup table using linear interpolation and a phase accumulator
  - Application provided generator

* No dynamic memory allocation
* Single-precision floating point only
* Simple configuration at compile-time via a `config.h` file.

* Unit tested with [GoogleTest](https://github.com/google/googletest).
* Benchmarked with [benchmark](https://github.com/google/benchmark).

## Building

You need [CMake >=3.16.3](https://cmake.org). Alternatively, you may copy the
files from `include` and `src` and integrate them manually into your workflow
for your specific use case.

You need a C compiler that at minimum supports the [C17 standard](https://en.wikipedia.org/wiki/C17_(C_standard_revision)).

You also need a C++ compiler that at minimum supports [C++14](https://en.wikipedia.org/wiki/C%2B%2B14)
if you plan on building the unit tests and benchmarks.

We highly recommend that you use either a [GNU Compiler Collection](https://gcc.gnu.org/) or
[Clang/LLVM](https://clang.llvm.org/) based compiler. Nothing is stopping you from using another
compiler, but warnings will be thrown by CMake as we don't have configurations
for other compilers. **This does necessarily mean it will fail to compile.**

All options are `OFF` by default; you must explicitly configure the build using
the CMake options below:

    -DLIBSAME_BUILD_BENCHMARKS:BOOL=ON/OFF
      ON:  Build the benchmarks. This requires benchmark which will be automatically
           fetched by CMake if benchmark cannot be found and LIBSAME_DOWNLOAD_DEPS is
           ON.

      OFF: The benchmarks will not be built. benchmark will not be downloaded or
           used for any reason.

    -DLIBSAME_BUILD_BENCHMARKS_WITH_SHARED_LIBRARY:BOOL=ON/OFF
      ON:  Build the benchmarks with the shared library form of libsame.
      OFF: Build the benchmarks with the static library form of libsame.

      CMake will throw an error if, for example, this option is ON and a shared
      library build wasn't requested. Likewise, if this option is OFF, CMake
      will throw an error if a static library build wasn't requested.

      This option has no effect is LIBSAME_BUILD_BENCHMARKS is OFF.

    -DLIBSAME_BUILD_EXAMPLES:BOOL=ON/OFF
      ON:  Build the examples. This requires SDL2 which will be automatically
           fetched by CMake if SDL2 cannot be found and LIBSAME_DOWNLOAD_DEPS is
           ON.

      OFF: The examples will not be built. SDL2 will not be downloaded or used
           for any reason.

    -DLIBSAME_BUILD_EXAMPLES_WITH_SHARED_LIBRARY:BOOL=ON/OFF
      ON:  Build the examples with the shared library form of libsame.
      OFF: Build the examples with the static library form of libsame.

      CMake will throw an error if, for example, this option is ON and a shared
      library build wasn't requested. Likewise, if this option is OFF, CMake
      will throw an error if a static library build wasn't requested.

      This option has no effect is LIBSAME_BUILD_EXAMPLES is OFF.

    -DLIBSAME_BUILD_TESTS:BOOL=ON/OFF
      ON:  Build the unit tests. This requires GoogleTest which will be
           automatically fetched by CMake due to GoogleTest adhering to the
           Abseil Live at Head philosophy. This will occur regardless of the
           LIBSAME_DOWNLOAD_DEPS setting if this option is ON.

      OFF: The unit tests will not be built. GoogleTest will not be downloaded
           or used for any reason.

    -DLIBSAME_BUILD_TESTS_WITH_SHARED_LIBRARY:BOOL=ON/OFF
      ON:  Build the unit tests with the shared library form of libsame.
      OFF: Build the unit tests with the static library form of libsame.

      CMake will throw an error if, for example, this option is ON and a shared
      library build wasn't requested. Likewise, if this option is OFF, CMake
      will throw an error if a static library build wasn't requested.

      This option has no effect is LIBSAME_BUILD_TESTS is OFF.

    -DLIBSAME_DOWNLOAD_MISSING_DEPS:BOOL=ON/OFF
      ON:  Automatically downloads missing dependencies as appropriate.
      OFF: Do not download any missing dependencies.

      This option has no effect if both LIBSAME_BUILD_BENCHMARKS and
      LIBSAME_BUILD_EXAMPLES are OFF.

    -DLIBSAME_ENABLE_CODE_COVERAGE:BOOL=ON/OFF
      ON:  Enables code coverage if supported by the compiler.
      OFF: Disables code coverage support.

    -DLIBSAME_ENABLE_SANITIZERS:BOOL=ON/OFF
      ON:  Enables both the AddressSanitizer (ASAN) and Undefined Behavior
           Sanitizer (UBSan) if supported by the compiler.

      OFF: Disables sanitizers.

    -DLIBSAME_BUILD_STATIC_LIBRARY:BOOL=ON/OFF
      ON:  Build a static library form of libsame.
      OFF: Do not build a static library form of libsame.

    -DLIBSAME_BUILD_SHARED_LIBRARY:BOOL=ON/OFF
      ON:  Build a shared library form of libsame.
      OFF: Do not build a shared library form of libsame.

    -DLIBSAME_WARNINGS_ARE_ERRORS:BOOL=ON/OFF
      ON:  Treat all compiler warnings as errors. Useful only for developers and
           CI pipelines.

      OFF: Do not treat all compiler warnings as errors.

Any standard CMake option (e.g., `CMAKE_BUILD_TYPE`, `CMAKE_INSTALL_PREFIX`) is
respected and can be safely passed.
