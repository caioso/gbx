#include <iostream>
#include <sstream>

import Logger;

#include "Version.h"

using namespace std;

int main (const int argc, const char* argv[])
{
    stringstream message;
    message << "GAMEBOY X Emulator " << SystemVersion();
    gbx::Log::LogLn(message.str().c_str());
}