#pragma once

#include <assert.h>

#include <algorithm>
#include <functional>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <random>
#include <string>
#include <vector>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long long ullong;

#define err(msg) {std::cerr << msg << std::endl; exit(1);}
#define print(msg) std::cout << msg << std::endl;
#define echo(msg) print(#msg << " = " << (msg));

inline ullong time_task(std::function<void ()> task) {
    const ullong NANO_SECONDS_IN_SEC = 1000000000;
    timespec t_start, t_end;

    clock_gettime(CLOCK_MONOTONIC, &t_start);
    task();
    clock_gettime(CLOCK_MONOTONIC, &t_end);

    t_end.tv_sec -= t_start.tv_sec;
    t_end.tv_nsec -= t_start.tv_nsec;
    if (t_end.tv_nsec < 0) {
        t_end.tv_sec--;
        t_end.tv_nsec += NANO_SECONDS_IN_SEC;
    }
    return ullong(t_end.tv_sec) * NANO_SECONDS_IN_SEC + ullong(t_end.tv_nsec);
}

inline uint parse_uint(const char *cstr,
                       const char *label,
                       uint minval = 0,
                       uint maxval = ~0) {
    char str[64];
    char *end;
    strncpy(str, cstr, 64);
    uint result = strtoul(str, &end, 0);
    if(*end) {
        if(*end == 'e') {
            char *exp_str = end + 1;
            uint exp = strtoul(exp_str, &end, 0);
            if(*end) {
                err("Invalid integer for " << label << ": '" << str << "'");
            }
            for(uint i = 0; i < exp; i++) {
                uint newval = result * 10;
                if(newval < result) {
                    err("uint overflow for " << label << ": '" << str << "'");
                }
                result = newval;
            }
        } else {
            err("Invalid integer for " << label << ": '" << str << "'");
        }
    }
    if( (result < minval) || (result > maxval) ) {
        err("Bounds error for " << label << ", '" << str << "' (" << result << ") not in range [" << minval << ", " << maxval << "]");
    }
    return result;
}
