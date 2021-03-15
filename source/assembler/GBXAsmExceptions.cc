#include "GBXAsmExceptions.h"

using namespace std;

namespace gbxasm
{

GBXAsmException::GBXAsmException(const string& message)
    : _message(message)
{}

PreProcessorException::PreProcessorException(const string& message)
    : GBXAsmException(message)
{}

LexicalAnalyzerException::LexicalAnalyzerException(const string& message)
    : GBXAsmException(message)
{}

SyntacticAnalyzerException::SyntacticAnalyzerException(const string& message)
    : GBXAsmException(message)
{}

ConversionException::ConversionException(const string& message)
    : GBXAsmException(message)
{}


const char* GBXAsmException::what() const noexcept
{
    return _message.c_str();
}

}