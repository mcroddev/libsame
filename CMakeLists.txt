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

  if (CMAKE_C_COMPILER_ID MATCHES "Clang" OR
      CMAKE_C_COMPILER_ID MATCHES "GNU")
    # These flags can be set regardless of compiler version.
    target_compile_options(libsame-build-settings-c INTERFACE
                           -fdata-sections
                           -ffunction-sections
                           -fstack-usage
                           -std=gnu90
                           -Wall
                           -Walloca
                           -Wcast-align
                           -Wcast-qual
                           -Wconversion
                           -Wdisabled-optimization
                           -Wdouble-promotion
                           -Werror
                           -Wextra
                           -Wformat=2
                           -Winline
                           -Wmissing-include-dirs
                           -Wmissing-noreturn
                           -Wno-error=disabled-optimization
                           -Wno-error=inline
                           -Wno-error=padded
                           -Wno-error=unused
                           -Wnull-dereference
                           -Wpadded
                           -Wpointer-arith
                           -Wshadow
                           -Wswitch-default
                           -Wundef
                           -Wvla
                           -Wwrite-strings)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(libsame-build-settings-c INTERFACE -O1)

      if (CMAKE_C_COMPILER_ID MATCHES "Clang")
        target_compile_options(libsame-build-settings-c INTERFACE -glldb)
      elseif (CMAKE_C_COMPILER_ID MATCHES "GNU")
        target_compile_options(libsame-build-settings-c INTERFACE -ggdb3)
      else()
        message(FATAL_ERROR
                "Don't know what CMAKE_C_COMPILER_ID is, but we're in a block "
                "that should guarantee that we know?")
      endif()
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      target_compile_options(libsame-build-settings-c INTERFACE -Ofast)
    else()
      message(WARNING
              "Unable to set project wide build settings for C targets: the "
              "build type ${CMAKE_BUILD_TYPE} is unhandled! libsame may not "
              "compile.")
    endif()
  else()
    message(WARNING
            "Unable to set project wide build settings for C targets: the C "
            "compiler ${CMAKE_C_COMPILER_ID} is unhandled! libsame may not "
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

  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    # These flags can be set regardless of compiler version.
    target_compile_options(libsame-build-settings-cpp INTERFACE
                           -fdata-sections
                           -ffunction-sections
                           -fstack-usage
                           -std=gnu++14
                           -Wall
                           -Walloca
                           -Wcast-align
                           -Wcast-qual
                           -Wconversion
                           -Wdisabled-optimization
                           -Wdouble-promotion
                           -Werror
                           -Wextra
                           -Wformat=2
                           -Winline
                           -Wmissing-include-dirs
                           -Wmissing-noreturn
                           -Wno-error=disabled-optimization
                           -Wno-error=inline
                           -Wno-error=padded
                           -Wno-error=unused
                           -Wnull-dereference
                           -Wpadded
                           -Wpointer-arith
                           -Wshadow
                           -Wswitch-default
                           -Wundef
                           -Wvla
                           -Wwrite-strings)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
      target_compile_options(libsame-build-settings-cpp INTERFACE -O1)

      if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(libsame-build-settings-cpp INTERFACE -glldb)
      elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_options(libsame_build-settings-cpp INTERFACE -ggdb3)
      else()
        message(FATAL_ERROR
                "Don't know what CMAKE_CXX_COMPILER_ID is, but we're in a block"
                " that should guarantee that we know?")
      endif()
    elseif (CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
      target_compile_options(libsame-build-settings-c INTERFACE -Ofast)
    else()
      message(WARNING
              "Unable to set project wide build settings for C++ targets: the "
              "build type ${CMAKE_BUILD_TYPE} is unhandled! Neither the unit "
              "tests nor the benchmarks may compile.")
    endif()
  else()
    message(WARNING
            "Unable to set project wide build settings for C++ targets: the C++"
            " compiler ${CMAKE_CXX_COMPILER_ID} is unhandled! Neither the unit "
            "tests nor the benchmarks may compile.")
  endif()
endfunction()

function(libsame_options_handle)
  option(LIBSAME_BUILD_TESTS "Build unit tests" OFF)
  option(LIBSAME_BUILD_BENCHMARKS "Build the benchmarks" OFF)
  option(LIBSAME_BUILD_EXAMPLES "Build the examples" OFF)

  option(LIBSAME_DOWNLOAD_DEPS "Download missing dependencies" OFF)

  option(LIBSAME_WARNINGS_ARE_ERRORS "Build libsame treating warnings as errors" OFF)

  option(LIBSAME_STATIC_LIBRARY "Build libsame as a static library" OFF)
  option(LIBSAME_SHARED_LIBRARY "Build libsame as a shared library" OFF)
endfunction()

function(libsame_tests_enable)
  include(FetchContent)

  # XXX: googletest follows the Abseil Live at Head philosophy, as such
  # regardless of LIBSAME_DOWNLOAD_DEPS being enabled or not, this is the
  # recommended way to pull in googletest.
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG main
  )

  # For Windows: Prevent overriding the parent project's compiler/linker
  # settings.
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  FetchContent_MakeAvailable(googletest)

  include(GoogleTest)
  enable_testing()
endfunction()

function(libsame_benchmarks_enable)
  find_package(benchmark QUIET)

  if (NOT benchmark_FOUND)
    if (NOT LIBSAME_DOWNLOAD_DEPS)
      message(FATAL_ERROR
              "benchmark was not found, and downloading missing dependencies "
              "is disabled.")
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
endfunction()

libsame_build_settings_c_configure()
libsame_options_handle()

if (LIBSAME_BUILD_TESTS OR LIBSAME_BUILD_BENCHMARKS)
  libsame_build_settings_cpp_configure()

  if (LIBSAME_BUILD_TESTS)
    libsame_tests_enable()
  endif()

  if (LIBSAME_BUILD_BENCHMARKS)
    libsame_benchmarks_enable()
  endif()
endif()

if (LIBSAME_ENABLE_LTO)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO TRUE)
endif()

# Build the library.
add_subdirectory(src)

if (LIBSAME_TESTS_BUILD)
  add_subdirectory(tests)
endif()

if (LIBSAME_BENCHMARKS_BUILD)
  add_subdirectory(benchmarks)
endif()
