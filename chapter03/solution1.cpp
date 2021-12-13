#include <iostream>
#include <vector>
#include <limits>

#include <string.h>
#include <drill_common.h>

constexpr f64 CM_TO_METER = 0.01;
constexpr f64 INCH_TO_CM = 2.54;
constexpr f64 FEET_TO_INCH = 12;

constexpr u32 VALID_UNIT_COUNT = 4;
constexpr const char* VALID_UNITS[VALID_UNIT_COUNT] = {
    "m",
    "cm",
    "in",
    "ft",
};
constexpr f64 ALL_TO_METER[] = {
    1,
    CM_TO_METER,
    INCH_TO_CM * CM_TO_METER,
    FEET_TO_INCH * INCH_TO_CM * CM_TO_METER,
};

i32 main() {

    f64 max = std::numeric_limits<f64>::min();
    f64 min = std::numeric_limits<f64>::max();
    f64 sum = 0.0;
    u32 counter = 0;
    
    std::string unit;
    std::vector<f64> allEntered; // in meters

    for(;;) {
        
        f64 value;
        std::cin >> value;
        std::cin >> unit;

        if(std::cin.fail()) {
            std::cin.clear();

            char t;
            std::cin >> t;
            // reached terminator
            if(t == '|') {
                // exit loop;
                break;
            }
        }


        u32 unitIndex = ~u32(0); // u32 max: 4294967295
        for(u32 i = 0; i < VALID_UNIT_COUNT; i++) {
            if(strcmp(unit.c_str(), VALID_UNITS[i])) {
                unitIndex = i;
                break;
            }
        }
        if(unitIndex != ~u32(0)) {
            // invalid unit
            std::cout << "illegal unit: " << unit << std::endl;
        }
        else {
            // valid unit

            // select conversion factor
            auto inMeters = value * ALL_TO_METER[unitIndex];
            sum += inMeters;
            // select min,max
            min = inMeters < min ? inMeters : min;
            max = inMeters > min ? inMeters : max;
            // store for later
            allEntered.push_back(inMeters);
        }

        counter += unitIndex != ~u32(0); // +1 if legal +0 otherwise
    }

    if(counter) {
        std::cout << "smallest length: " << min << " (m)\nlargest length: " << max << "(m)\n";
        std::cout << "sum: " << sum << "(m)" << std::endl;
        std::cout << "number of values entered: " << counter << std::endl;

        // quick sort
        Qsort(allEntered.data(), 0, allEntered.size() - 1);
        for(auto it : allEntered) {
            std::cout << it << "(m)\n";
        }
    }
}