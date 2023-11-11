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

include(FetchContent)

# XXX: googletest follows the Abseil Live at Head philosophy, as such
# regardless of LIBSAME_DOWNLOAD_DEPS being ON or not, this is the recommended
# way to pull in googletest.
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG main
)

# For Windows: Prevent overriding the parent project's compiler/linker settings.
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

if (LIBSAME_BUILD_TESTS_WITH_SHARED_LIBRARY)
  if (NOT LIBSAME_BUILD_SHARED_LIBRARY)
    message(FATAL_ERROR
            "libsame: The unit tests were asked to be linked with the shared "
            "library form of libsame, but the shared library form of libsame "
            "wasn't asked to be compiled. Pass "
            "-DLIBSAME_BUILD_SHARED_LIBRARY:BOOL=ON to your CMake invocation.")
  else()
    set(SAME_LIB sharedsame)
  endif()
else()
  if (NOT LIBSAME_BUILD_STATIC_LIBRARY)
    message(FATAL_ERROR
            "libsame: The unit tests were asked to be linked with the static "
            "library form of libsame, but the static library form of libsame "
            "wasn't asked to be compiled. Pass "
            "-DLIBSAME_BUILD_STATIC_LIBRARY:BOOL=ON to your CMake invocation.")
  else()
    set(SAME_LIB staticsame)
  endif()
endif()

function(libsame_test_add TEST_NAME TEST_SRC)
  add_executable(${TEST_NAME} ${TEST_SRC})

  target_link_libraries(${TEST_NAME} PRIVATE
                        ${SAME_LIB}
                        libsame-build-settings-cpp
                        GTest::gtest_main)

  gtest_discover_tests(${TEST_NAME})
endfunction()

libsame_test_add(libsame_afsk_gen libsame_afsk_gen.cpp)
libsame_test_add(libsame_attn_sig_gen libsame_attn_sig_gen.cpp)
libsame_test_add(libsame_ctx_init libsame_ctx_init.cpp)
libsame_test_add(libsame_data libsame_data.cpp)
libsame_test_add(libsame_field_add libsame_field_add.cpp)
libsame_test_add(libsame_silence_gen libsame_silence_gen.cpp)
libsame_test_add(libsame_sin libsame_sin.cpp)
