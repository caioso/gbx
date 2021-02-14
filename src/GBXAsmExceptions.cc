#include "GBXAsmExceptions.h"

using namespace std;

namespace gbxasm
{

GBXAsmException::GBXAsmException(const std::string message)
    : _message(message)
{}

LexicalAnalyzerException::LexicalAnalyzerException(const std::string message)
    : GBXAsmException(message)
{}

SyntacticAnalyzerException::SyntacticAnalyzerException(const std::string message)
    : GBXAsmException(message)
{}


const char* GBXAsmException::what() const noexcept
{
    return _message.c_str();
}

}