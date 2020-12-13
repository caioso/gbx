#include <iostream>
#include <sstream>

#include "Version.h"
#include "Logger.h"
#include "ReadOnlyMemory.h"

using namespace std;
using namespace gbx;

int main ()
{
    stringstream message;
    message << "GAMEBOY X Emulator " << SystemVersion();
    Log::LogLn(message.str().c_str());

    MemoryBase _rom(100);
    message.str(string());
    message << "Creating ROM array " << _rom.Size() << " bytes";
    Log::LogLn(message.str().c_str());

}