#include "ArithmeticLogicUnit.h"

using namespace std;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : ALUControlUnitChannel(make_shared<Channel<ALUMessage>>(ChannelType::InOut))
    , ALUMemoryControllerChannel(make_shared<Channel<MemoryMessage>>(ChannelType::InOut))
    , _state(ALUState::Idle)
{
    ALUControlUnitChannel->OnReceived([this](ALUMessage message) -> void { this->OnControlUnitMessage(message); });
    ALUMemoryControllerChannel->OnReceived([this](MemoryMessage message) -> void { this->OnMemoryControllerMessage(message); });

    InitializeRegisters();
}

void ArithmeticLogicUnit::OnControlUnitMessage(ALUMessage message)
{
    switch(message)
    {
        case ALUMessage::FetchPC: HandleControlSignalFetchPC(); break;
        case ALUMessage::Decode: HandleControlUnitSignalDecode(); break;
    }
}

void ArithmeticLogicUnit::OnMemoryControllerMessage(MemoryMessage message)
{
    switch(_state)
    {
        case ALUState::FetchingPC: HandleMemoryResponseFetchPC(message); break;
        case ALUState::Idle:
        default:
            throw ArithmeticLogicUnitException("unexpected memory response received");
    }
}

void ArithmeticLogicUnit::HandleControlSignalFetchPC()
{
    if (_state != ALUState::Idle)
        throw ArithmeticLogicUnitException("PC fetching can only be requested while the ALU is in Idle state");

    _state = ALUState::FetchingPC;
    
    auto programCounter = _registers.ReadPair(Register::PC);
    ALUMemoryControllerChannel->Send({MemoryRequestType::Read, programCounter, MemoryAccessType::Byte});
    
}

void ArithmeticLogicUnit::HandleControlUnitSignalDecode()
{
    if (_state != ALUState::FetchingPC)
        throw ArithmeticLogicUnitException("Decoding can only happen while the ALU in is FetchingPC state");

    _state = ALUState::Idle;
    DecodeInstruction();
    ALUControlUnitChannel->Send(ALUMessage::Complete);
}

void ArithmeticLogicUnit::DecodeInstruction()
{
    cout << "\tDecoding Instruction" << '\n';
}

void ArithmeticLogicUnit::HandleMemoryResponseFetchPC(MemoryMessage message)
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