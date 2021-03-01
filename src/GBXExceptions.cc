#include "GBXExceptions.h"

using namespace std;

namespace gbx
{

GBXException::GBXException(const std::string& message)
    : _message(message)
{}

RequestHandlerException::RequestHandlerException(const std::string& message)
    : GBXException(message)
{}

DebugServerException::DebugServerException(const std::string& message)
    : GBXException(message)
{}

const char* GBXException::what() const noexcept
{
    return _message.c_str();
}

}