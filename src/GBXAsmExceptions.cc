#include "GBXAsmExceptions.h"

using namespace std;

namespace gbxasm
{

GBXAsmException::GBXAsmException(const std::string message)
    : _message(message)
{}

LexerException::LexerException(const std::string message)
    : GBXAsmException(message)
{}


const char* GBXAsmException::what() const noexcept
{
    return _message.c_str();
}

}