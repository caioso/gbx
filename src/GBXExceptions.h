#pragma once

#include <exception>
#include <string>

namespace gbx
{

class GBXException : std::exception
{
public:
    GBXException(const std::string message);
    virtual ~GBXException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class MemoryAccessException : public GBXException
{
public:
    MemoryAccessException(const std::string message);
    ~MemoryAccessException() = default;
};

class MemoryControllerException : public GBXException
{
public:
    MemoryControllerException(const std::string message);
    ~MemoryControllerException() = default;
};

class ClockSourceException : public GBXException
{
public:
    ClockSourceException(const std::string message);
    ~ClockSourceException() = default;
};

class ChannelException : public GBXException
{
public:
    ChannelException(const std::string message);
    ~ChannelException() = default;
};

class ArithmeticLogicUnitException : public GBXException
{
public:
    ArithmeticLogicUnitException(const std::string message);
    ~ArithmeticLogicUnitException() = default;
};

class RegisterBankException : public GBXException
{
public:
    RegisterBankException(const std::string message);
    ~RegisterBankException() = default;
};

class InstructionException : public GBXException
{
public:
    InstructionException(const std::string message);
    ~InstructionException() = default;
};

}