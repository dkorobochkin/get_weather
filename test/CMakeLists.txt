cmake_minimum_required (VERSION 3.8)

project(get_weather_test)

set(GTEST_DIR ${THIRDPARTY_DIR}/googletest)

include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${THIRDPARTY_DIR}/nlohmann_json/include)
include_directories(${GTEST_DIR}/googletest/include)
include_directories(${GTEST_DIR}/googlemock/include)

link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

file(GLOB SOURCES_TEST "*.cpp")
file(GLOB HEADERS_TEST "*.h")

add_executable(get_weather_test ${SOURCES_TEST} ${HEADERS_TEST})

target_link_libraries(get_weather_test get_weather_lib gtest gmock)
if(WIN32)
	target_link_libraries(get_weather_test ws2_32)
endif()
