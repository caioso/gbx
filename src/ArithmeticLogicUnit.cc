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
        case ALUMessage::Fetch: HandleControlSignalFetch(); break;
        case ALUMessage::Decode: HandleControlUnitSignalDecode(); break;
        case ALUMessage::Execute: HandleControlUnitSignalExecute(); break;
        case ALUMessage::Acquire: HandleControlUnitSignalAcquire(); break;
        default: break;
    }
}

void ArithmeticLogicUnit::OnMemoryControllerMessage(MemoryMessage message)
{
    switch(_state)
    {
        case ALUState::FetchingPC: HandleMemoryResponseFetchPC(message); break;
        case ALUState::FethcingOperand1: HandleMemoryResponseFetchOperand1(message); break;
        default:
            throw ArithmeticLogicUnitException("unexpected memory response received");
    }
}

void ArithmeticLogicUnit::HandleControlSignalFetch()
{
    auto programCounter = _registers.ReadPair(Register::PC);

    _state = ALUState::FetchingPC;
    
    ALUMemoryControllerChannel->Send({MemoryRequestType::Read, programCounter, MemoryAccessType::Byte});
    _registers.WritePair(Register::PC, programCounter + 1);

    ALUControlUnitChannel->Send(ALUMessage::ReadyToDecode);
}

void ArithmeticLogicUnit::HandleControlUnitSignalDecode()
{
    _state = ALUState::Decoding;
    DecodeInstruction();
    DecideToAcquireOrExecute();
}

void ArithmeticLogicUnit::HandleControlUnitSignalAcquire()
{
    auto programCounter = _registers.ReadPair(Register::PC);

    _state = ALUState::FethcingOperand1;

    ALUMemoryControllerChannel->Send({MemoryRequestType::Read, programCounter, MemoryAccessType::Byte});
    _registers.WritePair(Register::PC, programCounter + 1);
    
    ALUControlUnitChannel->Send(ALUMessage::ReadyToExecute);
}

void ArithmeticLogicUnit::HandleControlUnitSignalExecute()
{
    _state = ALUState::Executing;
    ExecuteInstruction();
    DecideToWriteBackOrFetchPC();
}

void ArithmeticLogicUnit::ExecuteInstruction()
{
    if (_currentInstruction.opcode == Instruction::ld)
    {
        if (_currentInstruction.addressingMode == AddressingMode::Register)
        {
            auto currentSourceValue = _registers.Read(_currentInstruction.source);
            _registers.Write(_currentInstruction.destination, currentSourceValue);
        }
        else if (_currentInstruction.addressingMode == AddressingMode::Immediate)
        {
            _registers.Write(_currentInstruction.destination, _currentInstruction.memoryOperand1);
        }
    }
}

void ArithmeticLogicUnit::DecideToAcquireOrExecute()
{
    if (_currentInstruction.addressingMode == AddressingMode::Register)
        ALUControlUnitChannel->Send(ALUMessage::ReadyToExecute);
    else if (_currentInstruction.addressingMode == AddressingMode::Immediate)
    {
        ALUControlUnitChannel->Send(ALUMessage::ReadyToAcquire);
    }
}

void ArithmeticLogicUnit::DecideToWriteBackOrFetchPC()
{
    ALUControlUnitChannel->Send(ALUMessage::ReadyToFetch);
}

void ArithmeticLogicUnit::DecodeInstruction()
{
    // This will move to a separate class
    auto currentInstruction = _registers.Read(Register::IR);
    _currentInstruction = Decode(currentInstruction);
}

DecodedInstruction ArithmeticLogicUnit::Decode(uint8_t opcode)
{
    auto prefix = (opcode >> 6);

    if (prefix == 0x01)
    {
        auto destination = _registers.FromInstructionSource((opcode >> 3) & 0x07);
        auto source = _registers.FromInstructionDestination((opcode) & 0x07); 
        return {Instruction::ld, AddressingMode::Register, 0x00, source, destination};
    }
    if (prefix == 0x00)
    {
        auto destination = _registers.FromInstructionDestination((opcode >> 3) & 0x07);
        return {Instruction::ld, AddressingMode::Immediate, 0x00, Register::NoRegiser, destination};
    }

    stringstream ss;
    throw ArithmeticLogicUnitException(ss.str());
}

void ArithmeticLogicUnit::HandleMemoryResponseFetchPC(MemoryMessage message)
{
    _registers.Write(Register::IR, get<uint8_t>(message.Data));
}

void ArithmeticLogicUnit::HandleMemoryResponseFetchOperand1(MemoryMessage message)
{
    _currentInstruction.memoryOperand1 = get<uint8_t>(message.Data);
}

void ArithmeticLogicUnit::InitializeRegisters()
{
    _registers.Write(Register::IR, 0x00);
    _registers.WritePair(Register::PC, 0x0000);
    _registers.Write(Register::F, 0x00);
}


}