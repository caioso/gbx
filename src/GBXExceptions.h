#pragma once

#include <exception>
#include <string>

namespace gbx
{

class GBXException : std::exception
{
public:
    explicit GBXException(const std::string&);
    virtual ~GBXException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class MemoryAccessException : public GBXException
{
public:
    explicit MemoryAccessException(const std::string&);
    ~MemoryAccessException() = default;
};

class MemoryControllerException : public GBXException
{
public:
    explicit MemoryControllerException(const std::string&);
    ~MemoryControllerException() = default;
};

class ClockSourceException : public GBXException
{
public:
    explicit ClockSourceException(const std::string&);
    ~ClockSourceException() = default;
};

class ChannelException : public GBXException
{
public:
    explicit ChannelException(const std::string&);
    ~ChannelException() = default;
};

class ArithmeticLogicUnitException : public GBXException
{
public:
    explicit ArithmeticLogicUnitException(const std::string&);
    ~ArithmeticLogicUnitException() = default;
};

class RegisterBankException : public GBXException
{
public:
    explicit RegisterBankException(const std::string&);
    ~RegisterBankException() = default;
};

class InstructionException : public GBXException
{
public:
    explicit InstructionException(const std::string&);
    ~InstructionException() = default;
};

}