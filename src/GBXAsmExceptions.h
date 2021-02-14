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

class PreprocessorException : public GBXAsmException
{
public:
    PreprocessorException(const std::string);
    ~PreprocessorException() = default;
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