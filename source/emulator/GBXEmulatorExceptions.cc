#include "GBXEmulatorExceptions.h"

using namespace std;

namespace gbx
{

GBXEmulatorExceptions::GBXEmulatorExceptions(const std::string& message)
    : _message(message)
{}

const char* GBXEmulatorExceptions::what() const noexcept
{
    return _message.c_str();
}

}