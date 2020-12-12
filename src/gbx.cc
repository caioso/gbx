#include <iostream>
#include <sstream>

#include "Logger.h"
#include "Version.h"

using namespace std;
using namespace gbx;

int main (const int argc, const char* argv[])
{
    stringstream message;
    message << "GAMEBOY X Emulator " << SystemVersion();
    Log::LogLn(message.str().c_str());
}