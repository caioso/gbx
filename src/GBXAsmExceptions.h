#pragma once

#include <exception>
#include <string>

namespace gbxasm
{

class GBXAsmException : std::exception
{
public:
    GBXAsmException(const std::string);
    virtual ~GBXAsmException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class InstructionParserException : public GBXAsmException
{
public:
    InstructionParserException(const std::string);
    ~InstructionParserException() = default;
};

}