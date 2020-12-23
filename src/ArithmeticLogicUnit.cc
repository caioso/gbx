#include "ArithmeticLogicUnit.h"

using namespace std;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : ALUControlUnitChannel(make_shared<Channel<ALUMessage>>(ChannelType::InOut))
    , ALUMemoryControllerChannel(make_shared<Channel<MemoryMessage>>(ChannelType::InOut))
    , _state(ALUState::Idle)
    , _instructionState(InstructionState::NoInstruction)
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
        case ALUMessage::Execute: HandleControlUnitSignalExecute(); break;
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

    DecodeInstruction();
    EvaluateInstructionDependencies();
    _state = ALUState::Decoding;
}

void ArithmeticLogicUnit::HandleControlUnitSignalExecute()
{
    if (_state != ALUState::Decoding || _instructionState != InstructionState::ReadyToExecute)
         throw ArithmeticLogicUnitException("Execute can only happen while the ALU in is Decoding state and the current instruction has no extra dependencies");

    ExecuteInstruction();
    _state = ALUState::Complete;
    _instructionState = InstructionState::NoInstruction;
}

void ArithmeticLogicUnit::ExecuteInstruction()
{
    auto currentInstruction = _registers.Read(Register::IR);
    if (auto opcode = (currentInstruction >> 6) & 0xFF;
        opcode == 0x01)                                  // LD r, r opcode
    {
        auto source = _registers.FromInstructionSource((currentInstruction >> 3) & 0x07);
        auto destination = _registers.FromInstructionDestination((currentInstruction) & 0x07); 
        auto currentSourceValue = _registers.Read(source);
        _registers.Write(destination, currentSourceValue);
    }
}

void ArithmeticLogicUnit::EvaluateInstructionDependencies()
{
    // Evaluate here _instructionState to see whether the current instruction requires ferther memory access.
}

void ArithmeticLogicUnit::DecodeInstruction()
{
    // This will move to a separate class
    auto currentInstruction = _registers.Read(Register::IR);
    if (auto opcode = (currentInstruction >> 6) & 0xFF;
        opcode == 0x01)                                  // LD r, r opcode
    {
        _instructionState = InstructionState::ReadyToExecute;
    }
    else // Any other instruction
    {
        stringstream ss;
        ss << "Unable to decode instruction " << currentInstruction;
        throw ArithmeticLogicUnitException(ss.str());
    }
}

void ArithmeticLogicUnit::HandleMemoryResponseFetchPC(MemoryMessage message)
{
    _registers.WritePair(Register::PC, _registers.ReadPair(Register::PC) + 1);
    _registers.Write(Register::IR, get<uint8_t>(message.Data));
}


void ArithmeticLogicUnit::InitializeRegisters()
{
    _registers.Write(Register::IR, 0x00);
    _registers.WritePair(Register::PC, 0x0000);
    _registers.Write(Register::F, 0x00);
}


}