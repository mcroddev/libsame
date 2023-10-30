# libsame

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) [![Build libsame](https://github.com/mcroddev/libsame/actions/workflows/build.yml/badge.svg)](https://github.com/mcroddev/libsame/actions/workflows/build.yml)

libsame is a [Specific Area Message Encoding (SAME)](https://en.wikipedia.org/wiki/Specific_Area_Message_Encoding) header generator.

**WARNING**: The author is not responsible for the misuse of this software. The
output of this software has the potential to activate EAS decoder systems in the
continental United States of America and its territories, along with various
offshore marine areas.

Do **not**:

- transmit the output of this software over any radio communication unless
  so authorized.
- play the output of this software in public areas.
- use this software where life and limb are in jeopardy; it has not been
  certified for this purpose.

If you are under U.S. jurisdiction, please read 
[Misuse of the Emergency Alert System (EAS) Sound](https://www.fcc.gov/enforcement/areas/misuse-eas-sound) for more information.

## Features

libsame was designed for a variety of targets, from low-powered embedded systems
to your standard every day workstation. Features include:

* Very high portability; the core is written in the [C programming language](https://en.wikipedia.org/wiki/C_(programming_language))
  targeting the [ANSI C](https://en.wikipedia.org/wiki/ANSI_C) standard and fully forwards compatible with C
  standards up to [C17](https://en.wikipedia.org/wiki/C17_(C_standard_revision)).

* Multiple [AFSK](https://en.wikipedia.org/wiki/Frequency-shift_keying#Audio_frequency-shift_keying) generation engines
    - [C standard library](https://en.wikipedia.org/wiki/C_standard_library) `sin(f)()` function. This is the default.
    - Low-order [Taylor series](https://en.wikipedia.org/wiki/Taylor_series); generally faster on modern systems.
    - Sine wave lookup table with phase accumulator and linear interpolation, generally faster on embedded targets.
    - Application provided generator

  **NOTE:** Any claim regarding performance is dependent on your target system;
            we can only speak in broad generalities.


* No dynamic memory allocation
* Single-precision floating point only
* Easy, simple configuration at compile-time via a `config.h` file.


* Unit tested with [GoogleTest](https://github.com/google/googletest).
* Benchmarked with [benchmark](https://github.com/google/benchmark).

## Building

You need [CMake >=3.16.3](https://cmake.org). Alternatively, you may copy the
files from `include` and `src` and integrate them manually into your workflow
for your specific use case.

You need a C compiler that is at minimum capable of ANSI C, which should be
basically any compiler in existence.

You also need a C++ compiler that is at minimum capable of C++14 if you plan on
building the unit tests and benchmarks.

We highly recommend that you use either a [GNU Compiler Collection](https://gcc.gnu.org/) or
[Clang/LLVM](https://clang.llvm.org/) based compiler. Nothing is stopping you from using another
compiler, but warnings will be thrown by CMake as we don't have configurations
for other compilers. **This does necessarily mean it will fail to compile.**

libsame will be built as a static library by default and neither the unit tests,
benchmarks, nor examples will be built unless you explicitly ask for them.

libsame specific CMake options:

    -DLIBSAME_BUILD_EXAMPLES:BOOL=ON/OFF
      Build the examples. This requires SDL2 which will be automatically fetched
      by CMake if SDL2 cannot be found and LIBSAME_DOWNLOAD_DEPS is enabled.

      Default is off.

    -DLIBSAME_BUILD_TESTS:BOOL=ON/OFF
      Build the unit tests; additionally, it enables the sanitizers and code
      coverage if supported by the compiler.

      This requires GoogleTest which will be automatically fetched by CMake due
      to GoogleTest adhering to the Abseil Live at Head philosophy. This will
      occur regardless of the LIBSAME_DOWNLOAD_DEPS setting if this option is
      enabled.

      Default is off.

    -DLIBSAME_BUILD_BENCHMARKS:BOOL=ON/OFF
      Build the benchmarks. This requires benchmark which will be automatically
      fetched by CMake if benchmark cannot be found and LIBSAME_DOWNLOAD_DEPS is
      enabled.

      Default is off.

    -DLIBSAME_DOWNLOAD_DEPS:BOOL=ON/OFF
      Automatically downloads missing dependencies as appropriate.

      Default is off.

    -DLIBSAME_USE_SANITIZERS:BOOL=ON/OFF
      Turns on both ASAN and UBSan if supported by the compiler.

      Default is off.

    -DLIBSAME_STATIC_LIBRARY:BOOL=ON/OFF
      Build a static library form of libsame.

      Default is on.

    -DLIBSAME_SHARED_LIBRARY:BOOL=ON/OFF
      Build a shared library form of libsame.

      Default is off.

    -DLIBSAME_TESTS_SHARED_LIBRARY:BOOL=ON/OFF
      If unit tests are enabled, use the shared library form of libsame when
      linking. Otherwise, use the static library form.

      Default is off.

    -DLIBSAME_BENCHMARKS_SHARED_LIBRARY:BOOL=ON/OFF
      If benchmarks are enabled, use the shared library form of libsame when
      linking. Otherwise, use the static library form.

      Default is off.

    -DLIBSAME_EXAMPLES_SHARED_LIBRARY:BOOL=ON/OFF
      If examples are enabled, use the shared library form of libsame when
      linking. Otherwise, use the static library form.

      Default is off.

    -DLIBSAME_WARNINGS_ARE_ERRORS:BOOL=ON/OFF
      Treat all compile warnings as errors. Useful only for developers and CI
      pipelines.

      Default is off.
