#include "LCDScrollYRegister.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDScrollYRegister::LCDScrollYRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}
     
void LCDScrollYRegister::Write(variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->ScrollBackgroundY(_value);
}

}