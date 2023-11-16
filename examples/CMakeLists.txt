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

find_package(SDL2 QUIET)

if (NOT ${SDL2_found})
  if (NOT LIBSAME_DOWNLOAD_DEPS)
    message(FATAL_ERROR
            "SDL2 was not found, and downloading missing dependencies is "
            "disabled.")
  else()
    include(FetchContent)

    FetchContent_Declare(
      SDL2
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-2.28.4
    )

    set(SDL_STATIC ON CACHE BOOL "" FORCE)
    set(SDL_SHARED OFF CACHE BOOL "" FORCE)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    set(SDL_ATOMIC OFF CACHE BOOL "" FORCE)
    set(SDL_AUDIO ON CACHE BOOL "" FORCE)
    set(SDL_VIDEO OFF CACHE BOOL "" FORCE)
    set(SDL_RENDER OFF CACHE BOOL "" FORCE)
    set(SDL_EVENTS ON CACHE BOOL "" FORCE)
    set(SDL_JOYSTICK OFF CACHE BOOL "" FORCE)
    set(SDL_HAPTIC OFF CACHE BOOL "" FORCE)
    set(SDL_HIDAPI OFF CACHE BOOL "" FORCE)
    set(SDL_POWER OFF CACHE BOOL "" FORCE)
    set(SDL_TIMERS ON CACHE BOOL "" FORCE)
    set(SDL_FILE OFF CACHE BOOL "" FORCE)
    set(SDL_CPUINFO ON CACHE BOOL "" FORCE)
    set(SDL_FILESYSTEM OFF CACHE BOOL "" FORCE)
    set(SDL_SENSOR OFF CACHE BOOL "" FORCE)
    set(SDL_LOCALE OFF CACHE BOOL "" FORCE)
    set(SDL_MISC OFF CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(SDL2)
  endif()
endif()

if (LIBSAME_BUILD_EXAMPLES_WITH_SHARED_LIBRARY)
  if (NOT LIBSAME_BUILD_SHARED_LIBRARY)
    message(FATAL_ERROR
            "libsame: The examples were asked to be linked with the shared "
            "library form of libsame, but the shared library form of libsame "
            "wasn't asked to be compiled. Pass "
            "-DLIBSAME_BUILD_SHARED_LIBRARY:BOOL=ON to your CMake invocation.")
  else()
    set(SAME_LIB sharedsame)
  endif()
else()
  if (NOT LIBSAME_BUILD_STATIC_LIBRARY)
    message(FATAL_ERROR
            "libsame: The examples were asked to be linked with the static "
            "library form of libsame, but the static library form of libsame "
            "wasn't asked to be compiled. Pass "
            "-DLIBSAME_BUILD_STATIC_LIBRARY:BOOL=ON to your CMake invocation.")
  else()
    set(SAME_LIB staticsame)
  endif()
endif()

function(example_add SRC NAME)
  add_executable(${NAME} ${SRC})

  target_link_libraries(${NAME} PRIVATE
                        ${SAME_LIB}
                        libsame-build-settings-c
                        SDL2::SDL2
                        SDL2::SDL2main)

  target_include_directories(${NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
endfunction()

example_add(basic.c libsame_basic_example)
