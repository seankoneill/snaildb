cmake_minimum_required(VERSION 3.14)

project(snaildb CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.x
  )

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
  )

FetchContent_MakeAvailable(Catch2 spdlog)

add_library(core "libs/core/db.cc")
target_include_directories(core PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/lsm_tree/")
target_compile_options(core PRIVATE -g)

add_library(lsm_tree "libs/lsm_tree/cbf.h" "libs/lsm_tree/lsm.cc" "libs/lsm_tree/sst.cc" "libs/lsm_tree/segment.cc")
target_include_directories(lsm_tree PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/lsm_tree/" "${CMAKE_CURRENT_SOURCE_DIR}/libs/util/")
target_compile_options(lsm_tree PRIVATE -g)

add_executable(snaildb "main.cc")
target_include_directories(snaildb PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/lsm_tree/" "${CMAKE_CURRENT_SOURCE_DIR}/libs/util")
target_link_libraries(snaildb PRIVATE core lsm_tree spdlog::spdlog)
target_compile_options(snaildb PRIVATE -g)

add_executable(snail-test "test/behaviour_test.cc" "test/segment_test.cc" "test/test_util.cc")
target_include_directories(snail-test PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/libs/lsm_tree/" "${CMAKE_CURRENT_SOURCE_DIR}/libs/util")
target_link_libraries(snail-test PRIVATE core lsm_tree Catch2::Catch2WithMain)
target_compile_options(snail-test PRIVATE -g)

add_test(NAME snail-test COMMAND snail-test)
