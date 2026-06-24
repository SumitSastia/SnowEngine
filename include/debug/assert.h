#pragma once

#include <cassert>
#include <iostream>

// REMOVE THIS DURING RELEASE-BUILD
#define SNOW_DEBUG

#ifdef SNOW_DEBUG 
    #define SNOW_ASSERT(x, msg)                        \
        if (!(x)) {                                    \
            std::cerr << "ASSERT FAILED!\n";           \
            std::cerr << "FILE: " << __FILE__ << '\n'; \
            std::cerr << "LINE: " << __LINE__ << '\n'; \
            std::cerr << "REASON: " << msg << '\n';    \
            std::abort();                              \
        }

#else
    #define SNOW_ASSERT(x,msg)
#endif
