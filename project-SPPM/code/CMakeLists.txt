# cmake_minimum_required (VERSION 3.0)
# project (SPPM)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w --std=c++11 -O2 -g")
# if (CMAKE_SYSTEM_NAME MATCHES "Linux")
# 	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
# endif ()
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ./bin)
# aux_source_directory(./src SOURCES)
# add_executable(sppm ${SOURCES})
cmake_minimum_required(VERSION 3.16)
project(SPPM LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

find_package(OpenMP REQUIRED)

add_subdirectory(libs)
add_subdirectory(src)

add_executable(${PROJECT_NAME} ./src/main.cpp)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        renderer)