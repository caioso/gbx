#pragma once

#include <array>

#include "RegisterBank.h"

namespace gbxdb::output
{

class OutputDriver
{
public:
    OutputDriver() = default;
    ~OutputDriver() = default;

    OutputDriver(const OutputDriver&) = default;
    OutputDriver(OutputDriver&&) = default;
    OutputDriver& operator=(const OutputDriver&) = default;
    OutputDriver& operator=(OutputDriver&&) = default;

    virtual void DisplayRegisterbank(std::array<uint8_t, gbxcore::RegisterBankSizeInBytes>) = 0;
};

}