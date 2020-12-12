#include "Logger.h"

using namespace std;

namespace gbx
{

void Logger::Log(const char* message)
{
#ifdef DEBUG
    cout << message;
#endif
}

void Logger::LogLn(const char* message)
{
#ifdef DEBUG
    cout << message << '\n';
#endif
}

}