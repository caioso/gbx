#pragma once

#include <exception>
#include <string>

namespace gbxcore
{

class GBXCoreException : std::exception
{
public:
    explicit GBXCoreException(const std::string&);
    virtual ~GBXCoreException() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

class MemoryAccessException : public GBXCoreException
{
public:
    explicit MemoryAccessException(const std::string&);
    ~MemoryAccessException() = default;
};

class MemoryControllerException : public GBXCoreException
{
public:
    explicit MemoryControllerException(const std::string&);
    ~MemoryControllerException() = default;
};

class BankedMemoryException : public GBXCoreException
{
public:
    explicit BankedMemoryException(const std::string&);
    ~BankedMemoryException() = default;
};

class ClockSourceException : public GBXCoreException
{
public:
    explicit ClockSourceException(const std::string&);
    ~ClockSourceException() = default;
};

class ChannelException : public GBXCoreException
{
public:
    explicit ChannelException(const std::string&);
    ~ChannelException() = default;
};

class ArithmeticLogicUnitException : public GBXCoreException
{
public:
    explicit ArithmeticLogicUnitException(const std::string&);
    ~ArithmeticLogicUnitException() = default;
};

class RegisterBankException : public GBXCoreException
{
public:
    explicit RegisterBankException(const std::string&);
    ~RegisterBankException() = default;
};

class InstructionException : public GBXCoreException
{
public:
    explicit InstructionException(const std::string&);
    ~InstructionException() = default;
};

}