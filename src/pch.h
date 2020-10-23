//
// pch.h
// Header for standard system include files.
//

#pragma once

#define _USE_MATH_DEFINES

#include <HTTPRequest.hpp>
#include <nlohmann/json.hpp>

#if defined(__linux__) || defined(__APPLE__)
#include <signal.h>
#elif _WIN32
#include "Windows.h"
#endif

#include <optional>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <chrono>
#include <mutex>
#include <map>
#include <limits>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <list>
#include <sstream>
#include <iomanip>

using namespace std::chrono_literals;

using Clock = std::chrono::system_clock;
using Time = std::chrono::time_point<Clock>;
