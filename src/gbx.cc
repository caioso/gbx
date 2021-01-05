#include <iostream>
#include <sstream>

#include "Version.h"
#include "Logger.h"
#include "GameBoyX.h"

using namespace std;
using namespace gbx;

int main ()
{
    stringstream message;
    message << "GAME BOY X Emulator " << SystemVersion();
    Log::LogLn(message.str().c_str());

    auto gbx = make_unique<GameBoyX>();
    gbx->Run();
}