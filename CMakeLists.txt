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

if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-tree builds are not allowed.")
endif()

# The minimum version of CMake required is the latest CMake version present in
# the repositories of the oldest currently supported Ubuntu LTS release.
#
# At the time of this writing:
#
# - the oldest currently supported Ubuntu LTS release is 20.04 (Focal Fossa),
#   released on April 23rd, 2020.
#
# - the CMake version present in their repositories is 3.16.3, which was
#   released on January 21st, 2020.
#
# The FATAL_ERROR option is accepted but ignored by CMake 2.6 and higher. It
# should be specified so CMake versions 2.4 and lower fail with an error instead
# of just a warning.
cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

project(libsame
        VERSION 1.0.0.0
        DESCRIPTION "Specific Area Message Encoding (SAME) header generator"
        HOMEPAGE_URL "https://github.com/mcroddev/libsame"
        LANGUAGES C CXX)

# Generate a "compile_commands.json" file containing the exact compiler calls
# for all translation units of a project in machine-readable form. This is
# required for certain IDEs to provide code completion, error checking, etc.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Configures the project wide build settings for C targets.
function(libsame_build_settings_c_configure)
  # When targets link to this library, the settings set here will be passed to
  # the target.
  add_library(libsame-build-settings-c INTERFACE)

  if (LIBSAME_BUILD_TESTS)
    target_compile_definitions(libsame-build-settings-c INTERFACE
                               LIBSAME_TESTING)
  endif()

  if (CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(libsame-build-settings-c INTERFACE
                           -fdata-sections
                           -ffunction-sections
                           -std=gnu17
                           -Wall
                           -Walloca
                           -Wcast-align
                           -Wcast-qual
                           -Wconversion
                           -Wdisabled-optimization
                           -Wdouble-promotion
                           -Wextra
                           -Wformat=2
                           -Winline
                           -Wmissing-include-dirs
                           -Wmissing-noreturn
                           -Wnull-dereference
                           -Wpadded
                           -Wpointer-arith
                           -Wshadow
                           -Wswitch-default
                           -Wundef
                           -Wvla
                           -Wwrite-strings)

    if (LIBSAME_OPTIMIZE_FOR_HOST AND NOT CMAKE_TOOLCHAIN_FILE)
      target_compile_options(libsame-build-settings-c INTERFACE -mtune=native
                                                                -march=native)
    endif()

    if ((CMAKE_C_COMPILER_ID MATCHES "Clang" AND
        CMAKE_C_COMPILER_VERSION GREATER_EQUAL "13.0.0") OR
        CMAKE_C_COMPILER_ID MATCHES "GNU")
      target_compile_options(libsame-build-settings-c INTERFACE -fstack-usage)
    endif()

    if (LIBSAME_WARNINGS_ARE_ERRORS)
      target_compile_options(libsame-build-settings-c INTERFACE -Werror)
    endif()

    if (LIBSAME_BUILD_BENCHMARKS)
      target_compile_options(libsame-build-settings-c INTERFACE
                             -fno-omit-frame-pointer)
    endif()

    if (LIBSAME_ENABLE_SANITIZERS)
      target_compile_options(libsame-build-settings-c INTERFACE
                             -fno-omit-frame-pointer
                             -fsanitize=address,undefined)

      target_link_options(libsame-build-settings-c INTERFACE
                          -fsanitize=address,undefined)
    endif()

    if (LIBSAME_BUILD_TESTS)
      target_compile_options(libsame-build-settings-c INTERFACE --coverage)
      target_link_options(libsame-build-settings-c INTERFACE --coverage)
    endif()

    if (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo|Release")
      target_compile_options(libsame-build-settings-c INTERFACE -Ofast)
    endif()
  else()
    message(WARNING
            "Unable to set project wide build settings for C targets: the C "
            "compiler ${CMAKE_C_COMPILER_ID} is unhandled! We may or may not "
            "compile.")
  endif()
endfunction()

# Configures the project wide build settings for C++ targets.
# This is really only useful for the unit tests and benchmarks.
function(libsame_build_settings_cpp_configure)
  # When targets link to this library, the settings set here will be passed to
  # the target.
  add_library(libsame-build-settings-cpp INTERFACE)

  if (LIBSAME_BUILD_TESTS)
    target_compile_definitions(libsame-build-settings-cpp INTERFACE
                               LIBSAME_TESTING)
  endif()

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(libsame-build-settings-cpp INTERFACE
                           -fdata-sections
                           -ffunction-sections
                           -std=gnu++14
                           -Wall
                           -Walloca
                           -Wcast-align
                           -Wcast-qual
                           -Wconversion
                           -Wdisabled-optimization
                           -Wdouble-promotion
                           -Wextra
                           -Wformat=2
                           -Winline
                           -Wmissing-include-dirs
                           -Wmissing-noreturn
                           -Wnull-dereference
                           -Wpadded
                           -Wpointer-arith
                           -Wshadow
                           -Wswitch-default
                           -Wundef
                           -Wvla
                           -Wwrite-strings)

    if (LIBSAME_OPTIMIZE_FOR_HOST AND NOT CMAKE_TOOLCHAIN_FILE)
      target_compile_options(libsame-build-settings-c INTERFACE -mtune=native
                                                                -march=native)
    endif()

    if ((CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND
        CMAKE_CXX_COMPILER_VERSION GREATER_EQUAL "13.0.0") OR
        CMAKE_CXX_COMPILER_ID MATCHES "GNU")
      target_compile_options(libsame-build-settings-cpp INTERFACE -fstack-usage)
    endif()

    target_link_options(libsame-build-settings-cpp INTERFACE
                        -Wl,--gc-sections,--Map=libsame.map)

    if (LIBSAME_WARNINGS_ARE_ERRORS)
      target_compile_options(libsame-build-settings-cpp INTERFACE -Werror)
    endif()

    if (LIBSAME_BUILD_BENCHMARKS)
      target_compile_options(libsame-build-settings-cpp INTERFACE
                             -fno-omit-frame-pointer)
    endif()

    if (LIBSAME_ENABLE_SANITIZERS)
      target_compile_options(libsame-build-settings-cpp INTERFACE
                             -fno-omit-frame-pointer
                             -fsanitize=address,undefined)

      target_link_options(libsame-build-settings-cpp INTERFACE
                          -fsanitize=address,undefined)
    endif()

    if (LIBSAME_BUILD_TESTS)
      target_compile_options(libsame-build-settings-cpp INTERFACE --coverage)
      target_link_options(libsame-build-settings-cpp INTERFACE --coverage)
    endif()

    if (CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo|Release")
      target_compile_options(libsame-build-settings-cpp INTERFACE -Ofast)
    endif()
  else()
    message(WARNING
            "Unable to set project wide build settings for C++ targets: the C++"
            " compiler ${CMAKE_CXX_COMPILER_ID} is unhandled! We may or may not"
            " compile.")
  endif()
endfunction()

function(libsame_options_handle)
  set(LIBSAME_GENERATION_ENGINE "libc"
      CACHE STRING "Specifies the generation engine to use.")

  option(LIBSAME_OPTIMIZE_FOR_HOST "Optimize for host system" OFF)
  option(LIBSAME_ENABLE_LTO "Enable link-time optimization" OFF)

  option(LIBSAME_BUILD_BENCHMARKS "Build the benchmarks" OFF)
  option(LIBSAME_BUILD_BENCHMARKS_WITH_SHARED_LIBRARY
         "Use the shared library form of libsame for benchmarking" OFF)

  option(LIBSAME_BUILD_EXAMPLES "Build the examples" OFF)
  option(LIBSAME_BUILD_EXAMPLES_WITH_SHARED_LIBRARY
         "Use the shared library form of libsame for examples" OFF)

  option(LIBSAME_BUILD_TESTS "Build the unit tests" OFF)
  option(LIBSAME_BUILD_TESTS_WITH_SHARED_LIBRARY
         "Use the shared library form of libsame for unit testing" OFF)

  option(LIBSAME_DOWNLOAD_MISSING_DEPS
         "Automatically download missing dependencies" OFF)

  option(LIBSAME_ENABLE_CODE_COVERAGE "Enable code coverage support" OFF)
  option(LIBSAME_ENABLE_SANITIZERS "Enable ASAN and UBSan sanitizers" OFF)

  option(LIBSAME_BUILD_SHARED_LIBRARY "Build libsame as a shared library" OFF)
  option(LIBSAME_BUILD_STATIC_LIBRARY "Build libsame as a static library" OFF)

  option(LIBSAME_WARNINGS_ARE_ERRORS
         "Build libsame treating warnings as errors" OFF)
endfunction()

libsame_options_handle()

if (LIBSAME_ENABLE_LTO)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

if (LIBSAME_GENERATION_ENGINE STREQUAL "libc")
  set(LIBSAME_CONFIG_SINE_USE_LIBC ON)
elseif (LIBSAME_GENERATION_ENGINE STREQUAL "TaylorSeries")
  set(LIBSAME_CONFIG_SINE_USE_TAYLOR ON)
elseif (LIBSAME_GENERATION_ENGINE STREQUAL "LUT")
  set(LIBSAME_CONFIG_SINE_USE_LUT ON)
  set(LIBSAME_CONFIG_SINE_LUT_SIZE 1024
      CACHE STRING "Specifies the number of sine wave lookup table entries")
elseif (LIBSAME_GENERATION_ENGINE STREQUAL "App")
  set(LIBSAME_CONFIG_SINE_USE_APP ON)
else()
  message(FATAL_ERROR
          "The specified generation engine ${LIBSAME_GENERATION_ENGINE} is not "
          "valid.")
endif()

libsame_build_settings_c_configure()
libsame_build_settings_cpp_configure()

add_subdirectory(src)

if (LIBSAME_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

if (LIBSAME_BUILD_BENCHMARKS)
  if (NOT CMAKE_BUILD_TYPE MATCHES "Release|MinSizeRel|RelWithDebInfo")
    message(WARNING
            "You are building the benchmarks for libsame in debug mode, which "
            "is completely pointless. Use Release/MinSizeRel/RelWithDebInfo "
            "build types instead.")
  endif()

  add_subdirectory(benchmarks)
endif()

if (LIBSAME_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
