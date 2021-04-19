#include "LCDScanLineYRegister.h"

using namespace std;
using namespace gbxcore::interfaces;

namespace gbxcore::memory::registers
{

LCDScanLineYRegister::LCDScanLineYRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}
    
void LCDScanLineYRegister::Write([[maybe_unused]] std::variant<uint8_t, uint16_t> value)
{}

void LCDScanLineYRegister::UpdateScanLineValue(uint8_t scanLine)
{
    _value = scanLine;
}

void LCDScanLineYRegister::ResetScanLineValue()
{
    _value = 0;
}

}
