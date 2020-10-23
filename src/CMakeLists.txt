cmake_minimum_required (VERSION 3.8)

include_directories(${THIRDPARTY_DIR}/HTTPRequest/include)
include_directories(${THIRDPARTY_DIR}/nlohmann_json/include)

file(GLOB MAIN_SOURCES "*.cpp")
file(GLOB MAIN_HEADERS "*.h")

list(FILTER MAIN_SOURCES EXCLUDE REGEX "Main.cpp")

add_executable(get_weather "Main.cpp" ${MAIN_SOURCES} ${HEADERS})
add_library(get_weather_lib STATIC ${MAIN_SOURCES} ${HEADERS})

if(WIN32)
	target_link_libraries(get_weather ws2_32)
endif()
