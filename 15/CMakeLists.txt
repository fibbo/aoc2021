cmake_minimum_required(VERSION 3.20)

project(day15)
set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED)
add_executable(${PROJECT_NAME} day15.cpp)

target_link_libraries(${PROJECT_NAME}
PRIVATE
${Boost_LIBRARY_DIRS})

target_include_directories(${PROJECT_NAME}
PRIVATE
${Boost_INCLUDE_DIRS})
