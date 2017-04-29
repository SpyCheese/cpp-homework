cmake_minimum_required(VERSION 2.8)

project(DZ4ZIPPER)

add_executable(dz4zipper
			   main.cpp
			   ArgumentsParser.cpp
			   ArgumentsParser.h
			   ZipUnzip.h
			   Zip.cpp
			   Unzip.cpp
)

install(TARGETS dz4zipper DESTINATION bin)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0 -O2 -Wall -Wextra -std=c++14 -pedantic")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_USE_CXX11_ABI=0 -g -fsanitize=address,undefined")
endif()

target_link_libraries(dz4zipper -lmyzip)
