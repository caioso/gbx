#include "ArithmeticLogicUnit.h"

using namespace std;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : ALUControlUnitChannel(make_shared<Channel<ALUMessage>>(ChannelType::InOut))
    , ALUMemoryControllerChannel(make_shared<Channel<MemoryMessage>>(ChannelType::InOut))
{
    ALUControlUnitChannel->OnReceived([this](ALUMessage message) -> void { this->OnControlUnitMessage(message); });
    ALUMemoryControllerChannel->OnReceived([this](MemoryMessage message) -> void { this->OnMemoryControllerMessage(message); });

    InitializeRegisters();
}

void ArithmeticLogicUnit::OnControlUnitMessage(ALUMessage message)
{
    auto programCounter = _registers.ReadPair(Register::PC);
    ALUMemoryControllerChannel->Send({MemoryRequestType::Read, programCounter});
}

void ArithmeticLogicUnit::OnMemoryControllerMessage(MemoryMessage message)
{
    _registers.WritePair(Register::PC, _registers.ReadPair(Register::PC) + 1);
    _registers.Write(Register::IR, get<uint8_t>(message.Data));

    cout << "PC: " <<  _registers.ReadPair(Register::PC) << '\n';
}

void ArithmeticLogicUnit::InitializeRegisters()
{
    _registers.Write(Register::IR, 0x00);
    _registers.WritePair(Register::PC, 0x0000);
    _registers.Write(Register::F, 0x00);
}


}