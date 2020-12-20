#include "GBXExceptions.h"

using namespace std;

namespace gbx
{

GBXException::GBXException(const std::string message)
    : _message(message)
{}

MemoryAccessException::MemoryAccessException(const std::string message)
    : GBXException(message)
{}

MemoryControllerException::MemoryControllerException(const std::string message)
    : GBXException(message)
{}

ClockSourceException::ClockSourceException(const std::string message)
    : GBXException(message)
{}

ChannelException::ChannelException(const std::string message)
    : GBXException(message)
{}

const char* GBXException::what() const noexcept
{
    return _message.c_str();
}

}