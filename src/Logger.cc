module;
#include <iostream>

module Logger; 

using namespace std;

namespace gbx
{

void Log::LogLn(const char* message)
{
#ifdef DEBUG
    cout << message << '\n';
#endif
}

}