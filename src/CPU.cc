#include "CPU.h"

using namespace std;

namespace gbx
{

CPU::CPU()
    : _registerBank(make_unique<RegisterBank>())
    , _internalROM(make_unique<ROM>(0x100))
{
    InitializeRegisterBank();
}

void CPU::Run()
{
    auto nextPC = _registerBank->ReadPair(Register::PC) + 0x0001;
    _registerBank->WritePair(Register::PC, nextPC);
}

void CPU::InitializeRegisterBank()
{
    _registerBank->WritePair(Register::PC, 0x0000);
    _registerBank->Write(Register::F, 0x00);
}

}