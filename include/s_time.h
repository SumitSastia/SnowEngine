#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace running::core {

    using engine_clock      = std::chrono::steady_clock;
    using engine_time_unit  = std::chrono::microseconds;
    using engine_clock_time = std::chrono::steady_clock::time_point;

    inline const char*  unit = "us";

    class Timer {

    public:

        Timer() {
            startTime = engine_clock::now();
        }

        void restart() {
            startTime = engine_clock::now();
        }

        void interval_start() { 
            intervalTime = engine_clock::now();
        }

        const std::string endInterval() {

            const engine_clock_time stopTime = engine_clock::now();
            const engine_time_unit  duration = std::chrono::duration_cast<engine_time_unit>(stopTime - intervalTime);

            intervalTime = stopTime;
            return (std::to_string(duration.count()) + unit);
        }

        const std::string end() {

            const engine_clock_time stopTime = engine_clock::now();
            const engine_time_unit  duration = std::chrono::duration_cast<engine_time_unit>(stopTime - startTime);

            return (std::to_string(duration.count()) + unit);
        }

    private:

        engine_clock_time startTime;
        engine_clock_time intervalTime;
    };

    inline Timer GlobalTimer {};
}

namespace running::globalTimer {

    using namespace running::core;

    // inline timer        globalTimer;
    // inline time_keyword lastIntervalTime;

    // Initialize Timer
    // inline void start() {

    //     globalTimer.startTime = std::chrono::high_resolution_clock::now();
    //     lastIntervalTime      = globalTimer.startTime;
    // }

    // // Get Time taken to process instructions from the Start of Program.
    // inline const std::string getTime() {

    //     const time_keyword stopTime = std::chrono::high_resolution_clock::now();
    //     const time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - globalTimer.startTime);

    //     return (std::to_string(duration.count()) + unit);
    // }

    // inline const int64_t getTimeCount() {

    //     const time_keyword stopTime = std::chrono::high_resolution_clock::now();
    //     const time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - globalTimer.startTime);

    //     return duration.count();
    // }

    // inline void startInterval() {
    //     lastIntervalTime = std::chrono::high_resolution_clock::now();
    // }

    // inline const std::string endInterval() {

    //     const time_keyword stopTime = std::chrono::high_resolution_clock::now();
    //     const time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - lastIntervalTime);

    //     lastIntervalTime = stopTime;

    //     return (std::to_string(duration.count()) + unit);
    // }

    // inline const int64_t endIntervalCount() {

    //     const time_keyword stopTime = std::chrono::high_resolution_clock::now();
    //     const time_unit    duration = std::chrono::duration_cast<time_unit>(stopTime - lastIntervalTime);

    //     lastIntervalTime = stopTime;

    //     return duration.count();
    // }
}