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

ProtocolException::ProtocolException(const string& message)
    : GBXDebuggerExceptions(message)
{}

CommandLineInputException::CommandLineInputException(const string& message)
    : GBXDebuggerExceptions(message)
{}

CommandLineOutputDriverException::CommandLineOutputDriverException(const string& message)
    : GBXDebuggerExceptions(message)
{}

const char* GBXDebuggerExceptions::what() const noexcept
{
    return _message.c_str();
}

}