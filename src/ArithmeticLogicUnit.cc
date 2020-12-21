#include "ArithmeticLogicUnit.h"

using namespace std;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : ALUControlUnitChannel(make_shared<Channel<ALUMessage>>(ChannelType::InOut))
    , ALUROMChannel(make_shared<Channel<ROMMessage>>(ChannelType::InOut))
{
    ALUControlUnitChannel->OnReceived([this](ALUMessage message) -> void { this->OnControlUnitMessage(message); });
    ALUROMChannel->OnReceived([this](ROMMessage message) -> void { this->OnROMMessage(message); });
}

void ArithmeticLogicUnit::OnControlUnitMessage(ALUMessage message)
{
    auto programCounter = _registers.ReadPair(Register::PC);
    ALUROMChannel->Send({ROMRequestType::Read, programCounter});
}

void ArithmeticLogicUnit::OnROMMessage(ROMMessage message)
{
    _registers.WritePair(Register::PC, _registers.ReadPair(Register::PC) + 1);
    _registers.Write(Register::IR, get<uint8_t>(message.Data));

    cout << "PC: " <<  _registers.ReadPair(Register::PC) << '\n';
}

}