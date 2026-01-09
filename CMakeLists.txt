cmake_minimum_required(VERSION 3.14)
project(lef_pe_light_parser LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(parsers
  src/pe_parser.cpp
  src/lef_parser.cpp
)

target_include_directories(parsers PUBLIC src)

add_executable(lef_pe_cli src/main.cpp)
target_link_libraries(lef_pe_cli PRIVATE parsers)

# Tests
include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.10
)
FetchContent_MakeAvailable(catch2)

add_executable(tests tests/test_parsers.cpp)
target_link_libraries(tests PRIVATE parsers Catch2::Catch2)
add_test(NAME unit-tests COMMAND tests)