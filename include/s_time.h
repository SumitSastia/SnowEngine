#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace running::core {

    using time_unit    = std::chrono::microseconds;
    using time_keyword = std::chrono::_V2::system_clock::time_point;

    inline const char*  unit = "us";

    struct timer {

        time_keyword startTime;

        timer() {
            startTime = std::chrono::high_resolution_clock::now();
        }

        void restart() {
            startTime = std::chrono::high_resolution_clock::now();
        }

        const std::string end() {

            time_keyword stopTime = std::chrono::high_resolution_clock::now();
            time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - startTime);

            startTime = stopTime;

            return (std::to_string(duration.count()) + unit);
        }
    };
}

namespace running::globalTimer {

    using namespace running::core;

    inline timer        globalTimer;
    inline time_keyword lastIntervalTime;

    // Initialize Timer
    inline void start() {

        globalTimer.startTime = std::chrono::high_resolution_clock::now();
        lastIntervalTime = globalTimer.startTime;
    }

    // Get Time taken to process instructions from the Start of Program.
    inline const std::string getTime() {

        time_keyword stopTime = std::chrono::high_resolution_clock::now();
        time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - globalTimer.startTime);

        return (std::to_string(duration.count()) + unit);
    }

    inline void startInterval() {
        lastIntervalTime = std::chrono::high_resolution_clock::now();
    }

    inline const std::string endInterval() {

        time_keyword stopTime = std::chrono::high_resolution_clock::now();
        time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - lastIntervalTime);

        lastIntervalTime = stopTime;

        return (std::to_string(duration.count()) + unit);
    }
}