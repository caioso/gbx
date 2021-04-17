#pragma once 

#include <variant>

#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDScrollYRegister : public gbxcore::interfaces::MemoryMappedRegister
{
public:
    LCDScrollYRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDScrollYRegister() = default;
    
    std::size_t Size() override;
    std::variant<uint8_t, uint16_t> Read() override;
    void Write(std::variant<uint8_t, uint16_t>) override;

private:
    inline uint8_t Extract8BitValue(std::variant<uint8_t, uint16_t>);

    uint8_t _value{};
    gbxcore::interfaces::VideoControllerInterface* _videoController;
};


}