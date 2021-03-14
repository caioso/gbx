#include "GBXEmulatorExceptions.h"

using namespace std;

namespace gbx
{

GBXEmulatorExceptions::GBXEmulatorExceptions(const std::string& message)
    : _message(message)
{}

MessageHandlerException::MessageHandlerException(const std::string& message)
    : GBXEmulatorExceptions(message)
{}

ServerProtocolException::ServerProtocolException(const std::string& message)
    : GBXEmulatorExceptions(message)
{}

const char* GBXEmulatorExceptions::what() const noexcept
{
    return _message.c_str();
}

}