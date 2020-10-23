//
// pch.h
// Header for standard system include files.
//

#pragma once

#define _USE_MATH_DEFINES

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <nlohmann/json.hpp>

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
