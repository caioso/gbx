#include "GBXAsmExceptions.h"

using namespace std;

namespace gbxasm
{

GBXAsmException::GBXAsmException(const std::string message)
    : _message(message)
{}

InstructionParserException::InstructionParserException(const std::string message)
    : GBXAsmException(message)
{}

ParsedNumberException::ParsedNumberException(const std::string message)
    : GBXAsmException(message)
{}

const char* GBXAsmException::what() const noexcept
{
    return _message.c_str();
}

}