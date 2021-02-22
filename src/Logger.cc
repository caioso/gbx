#include "Logger.h"

namespace gbxcore
{

void Log::LogLn(const char* message)
{
#ifdef DEBUG
    cout << message << '\n';
#endif
}

}