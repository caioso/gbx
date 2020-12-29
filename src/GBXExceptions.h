#pragma once

#include <exception>
#include <string>

namespace gbx
{

class GBXException : std::exception
{
public:
    GBXException(const std::string);
    virtual ~GBXException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class MemoryAccessException : public GBXException
{
public:
    MemoryAccessException(const std::string);
    ~MemoryAccessException() = default;
};

class MemoryControllerException : public GBXException
{
public:
    MemoryControllerException(const std::string);
    ~MemoryControllerException() = default;
};

class ClockSourceException : public GBXException
{
public:
    ClockSourceException(const std::string);
    ~ClockSourceException() = default;
};

class ChannelException : public GBXException
{
public:
    ChannelException(const std::string);
    ~ChannelException() = default;
};

class ArithmeticLogicUnitException : public GBXException
{
public:
    ArithmeticLogicUnitException(const std::string);
    ~ArithmeticLogicUnitException() = default;
};

class RegisterBankException : public GBXException
{
public:
    RegisterBankException(const std::string);
    ~RegisterBankException() = default;
};

class InstructionException : public GBXException
{
public:
    InstructionException(const std::string message);
    ~InstructionException() = default;
};

}