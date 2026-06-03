#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace running::time {

    using time_unit    = std::chrono::microseconds;
    using time_keyword = std::chrono::_V2::system_clock::time_point;

    inline const char*  unit = "ms";

    inline time_keyword startTime;
    inline time_keyword lastIntervalTime;

    // Initialize Timer
    inline void start() {

        startTime = std::chrono::high_resolution_clock::now();
        lastIntervalTime = startTime;
    }

    // Get Time taken to process instructions from the Start of Program.
    inline const std::string getTime() {

        time_keyword stopTime = std::chrono::high_resolution_clock::now();
        time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - startTime);

        return (std::to_string(duration.count()) + unit);
    }

    inline void startInterval() {
        lastIntervalTime = std::chrono::high_resolution_clock::now();
    }

    inline const std::string endInterval() {

        time_keyword stopTime = std::chrono::high_resolution_clock::now();
        time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - lastIntervalTime);

        return (std::to_string(duration.count()) + unit);
    }
}