#include "LCDBackgroundScrollYRegister.h"

using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDBackgroundScrollYRegister::LCDBackgroundScrollYRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}
     
void LCDBackgroundScrollYRegister::Write(variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->ScrollBackgroundY(_value);
}

}