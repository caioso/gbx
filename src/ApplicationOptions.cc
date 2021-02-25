#include "ApplicationOptions.h"

using namespace std;

namespace gbx
{

string ApplicationHelp::Help()
{
    stringstream ss;
    ss << "Usage: gbx [-d/--debug -i/--ip <IPAddress> -p/--port <Port>]\n";
    ss << "options:\n";
    ss << " -d/--debug\t\tEnable debug mode\n";
    ss << " -i/--ip <IPAddress>\tDebug Server IP Address\n";
    ss << " -p/--port <Port>\tDebug Server Port Number\n";
    ss << " -v/--verbose\t\tVerbose mode\n";
    ss << " -h/--help\t\tShow this message\n";

    return ss.str();
}

}