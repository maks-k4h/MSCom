cmake_minimum_required(VERSION 3.21)
project(Compressor)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Compressor
        main.cpp   #comment to run tests
        MSComp/MSComp.cpp
        MSComp/MSComp.h
        MSComp/lib/nave96.cpp
        MSComp/lib/nave96.h
        MSComp/lib/Encoder.cpp
        MSComp/lib/Encoder.h
        tests/EncoderTests.cpp
        tests/nave96Tests.cpp
        )
target_link_libraries(Compressor gtest gtest_main)

