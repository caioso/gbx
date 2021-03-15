#pragma once

#include <exception>
#include <string>

namespace gbx
{

class GBXEmulatorExceptions : std::exception
{
public:
    explicit GBXEmulatorExceptions(const std::string&);
    virtual ~GBXEmulatorExceptions() = default;
    virtual const char* what() const noexcept;

protected:
    std::string _message;
};

}
