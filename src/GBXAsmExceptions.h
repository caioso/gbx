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

class PreProcessorException : public GBXAsmException
{
public:
    PreProcessorException(const std::string);
    ~PreProcessorException() = default;
};

class LexicalAnalyzerException : public GBXAsmException
{
public:
    LexicalAnalyzerException(const std::string);
    ~LexicalAnalyzerException() = default;
};

class SyntacticAnalyzerException : public GBXAsmException
{
public:
    SyntacticAnalyzerException(const std::string);
    ~SyntacticAnalyzerException() = default;
};
}