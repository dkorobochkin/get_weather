cmake_minimum_required (VERSION 3.8)

project("get_weather")

set(CMAKE_CXX_STANDARD 17)

set(THIRDPARTY_DIR ${CMAKE_SOURCE_DIR}/thirdparty)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
endif()

add_subdirectory("thirdparty/googletest")
add_subdirectory("src")
add_subdirectory("test")
