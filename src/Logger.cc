#include "Logger.h"

namespace gbx
{

void Log::LogLn(const char* message)
{
#ifdef DEBUG
    cout << message << '\n';
#endif
}

}