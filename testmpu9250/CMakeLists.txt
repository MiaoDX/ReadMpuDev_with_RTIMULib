cmake_minimum_required (VERSION 2.6)
project (testmpuDev)

#add_compile_options(-std=c++11)


set(CMAKE_BUILD_TYPE "Debug")
if( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -Wno-unused-variable -pthread")
else( CMAKE_BUILD_TYPE STREQUAL "Debug" )
        set(CMAKE_BUILD_TYPE "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread -fopenmp")
endif( CMAKE_BUILD_TYPE STREQUAL "Debug" )
message("Build Type:" ${CMAKE_BUILD_TYPE} ${CMAKE_CXX_FLAGS})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/RTIMULib)

ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/RTIMULib RTIMULib)


SET(DRIVE_SRCS
    myMath.h ReadMpuDev.h ReadMpuDev.cpp testmpuDev.cpp)

ADD_EXECUTABLE(testmpuDev ${DRIVE_SRCS})
TARGET_LINK_LIBRARIES(testmpuDev RTIMULib pthread)
