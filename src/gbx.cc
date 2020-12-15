#include <iostream>
#include <sstream>

#include "Version.h"
#include "Logger.h"
#include "CPU.h"

using namespace std;
using namespace gbx;

int main ()
{
    stringstream message;
    message << "GAME BOY X Emulator " << SystemVersion();
    Log::LogLn(message.str().c_str());
}