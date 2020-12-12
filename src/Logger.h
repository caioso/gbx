#pragma once

#include <iostream>

namespace gbx
{

class Logger
{
public:
    static void Log(const char* message);
    static void LogLn(const char* message);
};

typedef Logger Log;
}