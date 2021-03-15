#include "GBXDebuggerExceptions.h"

using namespace std;

namespace gbxdb
{

GBXDebuggerExceptions::GBXDebuggerExceptions(const string& message)
    : _message(message)
{}

MessageHandlerException::MessageHandlerException(const string& message)
    : GBXDebuggerExceptions(message)
{}

ServerProtocolException::ServerProtocolException(const string& message)
    : GBXDebuggerExceptions(message)
{}

const char* GBXDebuggerExceptions::what() const noexcept
{
    return _message.c_str();
}

}