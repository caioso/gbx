#pragma once 

#include <variant>

#include "EightBitMemoryMappedRegisterBase.h"
#include "MemoryMappedRegister.h"
#include "VideoControllerInterface.h"

namespace gbxcore::memory::registers
{

class LCDScanLineYRegister : public EightBitMemoryMappedRegisterBase
{
public:
    LCDScanLineYRegister(gbxcore::interfaces::VideoControllerInterface*);
    virtual ~LCDScanLineYRegister() = default;
    
    // Write is the method used outside of the video controller (thus as seen in the memory controller)
    void Write(std::variant<uint8_t, uint16_t>) override;

    // This method gets called by the video controller, as the scan lines are updated during ther screen update.
    void UpdateScanLineValue(uint8_t);
    // When LCD Control register bit 7 is set to 0, the value of this register becomes 0
    void ResetScanLineValue();

private:
    [[maybe_unused]] gbxcore::interfaces::VideoControllerInterface* _videoController;
};


}