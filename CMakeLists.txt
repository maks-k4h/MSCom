cmake_minimum_required(VERSION 3.21)
project(MSCom)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(tests/googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(Compressor
        main.cpp   #comment to run tests
        MSCom/MSCom.cpp
        MSCom/MSCom.h
        MSCom/ComMeasurements.cpp
        MSCom/ComMeasurements.h
        MSCom/lib/nave96.cpp
        MSCom/lib/nave96.h
        MSCom/lib/Encoder.cpp
        MSCom/lib/Encoder.h
        MSCom/lib/Decoder.cpp
        MSCom/lib/Decoder.h

        tests/EncoderTests.cpp
        tests/DecoderTests.cpp
        tests/nave96Tests.cpp
        )
target_link_libraries(Compressor gtest gtest_main)

