#include "GBXCoreExceptions.h"

using namespace std;

namespace gbxcore
{

GBXCoreException::GBXCoreException(const std::string& message)
    : _message(message)
{}

MemoryAccessException::MemoryAccessException(const std::string& message)
    : GBXCoreException(message)
{}

MemoryControllerException::MemoryControllerException(const std::string& message)
    : GBXCoreException(message)
{}

BankedMemoryException::BankedMemoryException(const std::string& message)
    : GBXCoreException(message)
{}

ClockSourceException::ClockSourceException(const std::string& message)
    : GBXCoreException(message)
{}

ChannelException::ChannelException(const std::string& message)
    : GBXCoreException(message)
{}

ArithmeticLogicUnitException::ArithmeticLogicUnitException(const std::string& message)
    : GBXCoreException(message)
{}

RegisterBankException::RegisterBankException(const std::string& message)
    : GBXCoreException(message)
{}

InstructionException::InstructionException(const std::string& message)
    : GBXCoreException(message)
{}

const char* GBXCoreException::what() const noexcept
{
    return _message.c_str();
}

}