# libsame

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

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

* High portability
  - Written in the [C programming language](https://en.wikipedia.org/wiki/C_(programming_language)),
    targeting the [ANSI C](https://en.wikipedia.org/wiki/ANSI_C) standard.
    - While not required, using a [GNU Compiler Collection](https://gcc.gnu.org/)
      or [Clang/LLVM](https://clang.llvm.org/) based compiler is highly
      recommended.
    - Fully forwards compatible with C standards up to [C17](https://en.wikipedia.org/wiki/C17_(C_standard_revision)).

* Multiple [AFSK](https://en.wikipedia.org/wiki/Frequency-shift_keying#Audio_frequency-shift_keying) generation engines
    - [C standard library](https://en.wikipedia.org/wiki/C_standard_library) `sinf()` function. This is the default.
    - Low-order [Taylor series](https://en.wikipedia.org/wiki/Taylor_series); generally faster on modern systems.
    - Sine wave lookup table with phase accumulator and linear interpolation, generally faster on embedded targets.
    - Application provided generator

  **NOTE:** Any claim regarding performance is dependent on your target system;
            we can only speak in broad generalities.


* No dynamic memory allocation; all sample generation is done in chunks.
* Single-precision floating point only
* Easy, simple configuration at compile-time via a `config.h` file.
* Unit tested with [GoogleTest](https://github.com/google/googletest).
* Benchmarked with [benchmark](https://github.com/google/benchmark).
