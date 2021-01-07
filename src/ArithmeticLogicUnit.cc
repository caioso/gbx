#include "ArithmeticLogicUnit.h"

using namespace std;
using namespace gbx::interfaces;

namespace gbx
{
void ArithmeticLogicUnit::Decode(uint8_t opcode, std::optional<uint8_t> preOpcode)
{
    try
    {
        _currentInstruction = _decoder.DecodeOpcode(opcode, preOpcode);
        _currentInstruction->Decode(opcode, preOpcode, InstructionData);
    }
    catch (const InstructionException&)
    {
        _currentInstruction.reset();
    }
}

void ArithmeticLogicUnit::Execute(std::shared_ptr<RegisterBank> registerBank)
{
    if (_currentInstruction == nullptr)
        throw InstructionException("tried to execute an andecoded instruction");

    _currentInstruction->Execute(registerBank, InstructionData);     
}

}