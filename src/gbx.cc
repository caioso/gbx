#include <iostream>
#include <sstream>

#include "Version.h"
#include "Logger.h"
#include "RegisterBank.h"

using namespace std;
using namespace gbx;

int main ()
{
    stringstream message;
    message << "GAME BOY X Emulator " << SystemVersion();
    Log::LogLn(message.str().c_str());

    RegisterBank registers;
    registers.Write(Register::PC, 0x0100);
    message.str(string(""));
    message << "Program Counter: 0x" << hex << registers.ReadPair(Register::PC);
    Log::LogLn(message.str().c_str());
}