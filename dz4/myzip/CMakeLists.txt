cmake_minimum_required(VERSION 2.8)

project(MYZIP)
include_directories(${MYZIP_SOURCE_DIR})

add_library(myzip
			BitSequence.cpp
			BitSequence.h
			Decoder.cpp
			Decoder.h
			Encoder.cpp
			Encoder.h
			Exceptions.h
			FrequencyDistribution.cpp
			FrequencyDistribution.h
			_HuffmanTree.cpp
			_HuffmanTree.h
)

install (TARGETS myzip DESTINATION lib)
install (FILES
	BitSequence.h
	Decoder.h
	Encoder.h
	Exceptions.h
	FrequencyDistribution.h
	_HuffmanTree.h
DESTINATION include/myzip)

add_executable(myzip_testing
               myzip_testing.cpp
               gtest/gtest-all.cc
               gtest/gtest.h
               gtest/gtest_main.cc
			   BitSequence.cpp
			   BitSequence.h
			   Decoder.cpp
			   Decoder.h
			   Encoder.cpp
			   Encoder.h
			   Exceptions.h
			   FrequencyDistribution.cpp
			   FrequencyDistribution.h
			   _HuffmanTree.cpp
			   _HuffmanTree.h
)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -std=c++14 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address,undefined")
endif()

target_link_libraries(myzip_testing -lgmp -lgmpxx -lpthread)
