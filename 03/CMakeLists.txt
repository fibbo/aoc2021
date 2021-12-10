cmake_minimum_required(VERSION 3.12)

project(day3)
set(CMAKE_CXX_STANDARD 20)
add_executable(${PROJECT_NAME} day3.cpp)
add_executable(test_${PROJECT_NAME} day3.cpp)

target_compile_definitions(test_${PROJECT_NAME} PRIVATE AOC_TEST)
target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
