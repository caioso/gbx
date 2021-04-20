#include "LCDWindowScrollYRegister.h"

using namespace gbxcore;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::memory::registers
{

LCDWindowScrollYRegister::LCDWindowScrollYRegister(VideoControllerInterface* controller)
    : _videoController(controller)
{}
     
void LCDWindowScrollYRegister::Write(variant<uint8_t, uint16_t> value)
{
    _value = Extract8BitValue(value);
    _videoController->ScrollWindowY(_value);
}

}