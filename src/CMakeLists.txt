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

include(CheckSymbolExists)
include(CheckIncludeFile)

function(lib_handle NAME)
  target_link_libraries(${NAME} PRIVATE libsame-build-settings-c m)
  target_include_directories(${NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(OUTPUT_NAME "samed")
  elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel|Release|RelWithDebInfo")
    set(OUTPUT_NAME "same")
  else()
    message(WARNING
            "Don't know the build type, cannot change output name of library.")
  endif()

  set_target_properties(${NAME} PROPERTIES OUTPUT_NAME ${OUTPUT_NAME})
endfunction()

check_symbol_exists(sinf "math.h" LIBSAME_HAVE_SINF)
check_include_file(stdint.h _STDINT_H_)

set(SRCS libsame.c)
set(HDRS ${PROJECT_SOURCE_DIR}/include/libsame/compiler.h
         ${PROJECT_SOURCE_DIR}/include/libsame/config.h
         ${PROJECT_SOURCE_DIR}/include/libsame/debug.h
         ${PROJECT_SOURCE_DIR}/include/libsame/libsame.h
)

# XXX: While we could compile the library as object files to avoid compiling
# twice (once for shared, once for static), this has the unfortunate
# disadvantage of forcing the static library to be compiled with position
# independent code, which is unnecessary and may have a small performance
# impact.

if (LIBSAME_BUILD_SHARED_LIBRARY)
  add_library(sharedsame SHARED ${SRCS} ${HDRS})
  lib_handle(sharedsame)
endif()

if (LIBSAME_BUILD_STATIC_LIBRARY)
  add_library(staticsame STATIC ${SRCS} ${HDRS})
  lib_handle(staticsame)
endif()
