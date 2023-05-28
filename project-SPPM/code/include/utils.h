#ifndef UTILITIES_H
#define UTILITIES_H
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <map>
#include <ctime>
#include <stdexcept>
#include "Config.h"

namespace Utils {
    vector<string> split(char *str);
    const int prime[] = {
            2,3,5,7,11,13,17,19,23,29,
            31,37,41,43,47,53,59,61,67,71,
            73,79,83,89,97,101,103,107,109,113
    };

    double randomQMC(int axis, long long step);
    static double random_double();
    double my_random(double min = 0.0f, double max = 1.0f, int axis = -1,
    long long step = 0);
}


#endif