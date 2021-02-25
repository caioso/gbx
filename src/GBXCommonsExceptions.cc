#include "GBXCommonsExceptions.h"

using namespace std;

namespace gbx
{

GBXCommonsException::GBXCommonsException(const std::string& message)
    : _message(message)
{}

ArgumentsParserException::ArgumentsParserException(const std::string& message)
    : GBXCommonsException(message)
{}

const char* GBXCommonsException::what() const noexcept
{
    return _message.c_str();
}

}