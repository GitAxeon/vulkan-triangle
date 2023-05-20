#pragma once
#include <iostream>

#define DEBUG

#ifdef DEBUG
    #define MESSAGE(type, msg) std::cout << #type << ": " << msg << "\n";
    #define LOG(x) MESSAGE(Log, x)
    #define ERROR(x) MESSAGE(Error, x)
#else
    #define LOG(x)
    #define ERROR(x)
#endif