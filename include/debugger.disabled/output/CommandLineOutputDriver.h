#pragma once 

#include <array>
#include <iostream>
#include <string>
#include <sstream>

#include "GBXDebuggerExceptions.h"
#include "OutputDriver.h"
#include "RegisterBank.h"
#include "RegisterBankInterface.h"

namespace gbxdb::output
{

class CommandLineOutputDriver : public gbxdb::interfaces::OutputDriver
{
public:
    CommandLineOutputDriver() = default;
    ~CommandLineOutputDriver() = default;

    CommandLineOutputDriver(const CommandLineOutputDriver&) = default;
    CommandLineOutputDriver(CommandLineOutputDriver&&) = default;
    CommandLineOutputDriver& operator=(const CommandLineOutputDriver&) = default;
    CommandLineOutputDriver& operator=(CommandLineOutputDriver&&) = default;

    void DisplayRegisterbank(std::array<uint8_t, gbxcore::RegisterBankSizeInBytes>) override;

private: 
    gbxcore::interfaces::Register ConvertIndexToRegister(size_t);
    std::string ConvertRegisterToRegisterName(gbxcore::interfaces::Register);
};

}