#include "ArithmeticLogicUnit.h"

using namespace std;

namespace gbx
{

ArithmeticLogicUnit::ArithmeticLogicUnit()
    : ALUControlUnitChannel(make_shared<Channel<ALUMessage>>(ChannelType::InOut))
    , ALUMemoryControllerChannel(make_shared<Channel<MemoryMessage>>(ChannelType::InOut))
    , _state(ALUState::Idle)
    , _registers(make_shared<RegisterBank>())
{
    ALUControlUnitChannel->OnReceived([this](ALUMessage message) -> void { this->OnControlUnitMessage(message); });
    ALUMemoryControllerChannel->OnReceived([this](MemoryMessage message) -> void { this->OnMemoryControllerMessage(message); });

    InitializeRegisters();
}

inline void ArithmeticLogicUnit::OnControlUnitMessage(ALUMessage message)
{
    switch(message)
    {
        case ALUMessage::Fetch: HandleControlSignalFetch(); break;
        case ALUMessage::Decode: HandleControlUnitSignalDecode(); break;
        case ALUMessage::Execute: HandleControlUnitSignalExecute(); break;
        case ALUMessage::Acquire: HandleControlUnitSignalAcquire(); break;
        case ALUMessage::WriteBack: HandleControlUnitSignalWriteBack(); break;
        default: break;
    }
}

inline void ArithmeticLogicUnit::OnMemoryControllerMessage(MemoryMessage message)
{
    switch(_state)
    {
        case ALUState::FetchingPC: HandleMemoryResponseFetchPC(message); break;
        case ALUState::FethcingOperand1: HandleMemoryResponseFetchOperand1(message); break;
        case ALUState::WritingBack: HandleMemoryResponseWriteBack(message); break;
        default:
            throw ArithmeticLogicUnitException("unexpected memory response received");
    }
}

inline void ArithmeticLogicUnit::HandleControlSignalFetch()
{
    _state = ALUState::FetchingPC;
    
    auto programCounter = _registers->ReadPair(Register::PC);
    ALUMemoryControllerChannel->Send({MemoryRequestType::Read, programCounter, static_cast<uint8_t>(0x00), MemoryAccessType::Byte});
    _registers->WritePair(Register::PC, programCounter + 1);

    ALUControlUnitChannel->Send(ALUMessage::ReadyToDecode);
}

inline void ArithmeticLogicUnit::HandleControlUnitSignalDecode()
{
    _state = ALUState::Decoding;
    DecodeInstruction();
    DecideToAcquireOrExecute();
}

inline void ArithmeticLogicUnit::HandleControlUnitSignalAcquire()
{
    _state = ALUState::FethcingOperand1;
    auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;

    if (currentAddressingMode == AddressingMode::Immediate)
    {
        // Move to function?
        auto programCounter = _registers->ReadPair(Register::PC);
        ALUMemoryControllerChannel->Send({MemoryRequestType::Read, programCounter, static_cast<uint8_t>(0x00), MemoryAccessType::Byte});
        _registers->WritePair(Register::PC, programCounter + 1);
    }
    else if (currentAddressingMode == AddressingMode::RegisterIndirectSource)
    {
        auto sourceContent = _registers->ReadPair(_currentInstruction->InstructionData.value().SourceRegister);
        ALUMemoryControllerChannel->Send({MemoryRequestType::Read, sourceContent, static_cast<uint8_t>(0x00), MemoryAccessType::Byte});
    }
    
    ALUControlUnitChannel->Send(ALUMessage::ReadyToExecute);
}

inline void ArithmeticLogicUnit::HandleControlUnitSignalExecute()
{
    _state = ALUState::Executing;
    ExecuteInstruction();
    DecideToWriteBackOrFetchPC();
}

inline void ArithmeticLogicUnit::HandleControlUnitSignalWriteBack()
{
    _state = ALUState::WritingBack;
    WriteBackResults();
    ALUControlUnitChannel->Send(ALUMessage::ReadyToFetch);
}

inline void ArithmeticLogicUnit::WriteBackResults()
{
    auto resultContent = _currentInstruction->InstructionData.value().MemoryResult1;
    auto resultAddress = _registers->ReadPair(_currentInstruction->InstructionData.value().DestinationRegister);
    ALUMemoryControllerChannel->Send({MemoryRequestType::Write, resultAddress, static_cast<uint8_t>(resultContent), MemoryAccessType::Byte});

}

inline void ArithmeticLogicUnit::ExecuteInstruction()
{
    // TODO: Maybe move this to another class
    if (_currentInstruction->InstructionData.value().Opcode == OpcodeType::ld)
        _currentInstruction->Execute(_registers, ALUMemoryControllerChannel);
}

inline void ArithmeticLogicUnit::DecideToAcquireOrExecute()
{
    auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;

    if (currentAddressingMode == AddressingMode::Register || 
        currentAddressingMode == AddressingMode::RegisterIndirectDestination)
        ALUControlUnitChannel->Send(ALUMessage::ReadyToExecute);
    else if (currentAddressingMode == AddressingMode::Immediate || 
             currentAddressingMode == AddressingMode::RegisterIndirectSource)
        ALUControlUnitChannel->Send(ALUMessage::ReadyToAcquire);
}

inline void ArithmeticLogicUnit::DecideToWriteBackOrFetchPC()
{
    if (_currentInstruction->InstructionData.value().AddressingMode == AddressingMode::RegisterIndirectDestination)
        ALUControlUnitChannel->Send(ALUMessage::ReadyToWriteBack);
    else
        ALUControlUnitChannel->Send(ALUMessage::ReadyToFetch);
}

inline void ArithmeticLogicUnit::DecodeInstruction()
{
    auto opcode = _registers->Read(Register::IR);
    Decode(opcode);
}

inline void ArithmeticLogicUnit::Decode(uint8_t opcode)
{
    auto prefix = (opcode >> 6);

    // Improve this
    if (prefix == 0x01 || prefix == 0x00)
    {
        _currentInstruction = make_unique<LD>();
        _currentInstruction->Decode(opcode);
    }
    else
    {
        stringstream ss;
        throw ArithmeticLogicUnitException(ss.str());
    }
}

inline void ArithmeticLogicUnit::HandleMemoryResponseFetchPC(MemoryMessage message)
{
    _registers->Write(Register::IR, get<uint8_t>(message.Data));
}

inline void ArithmeticLogicUnit::HandleMemoryResponseFetchOperand1(MemoryMessage message)
{
    auto currentAddressingMode = _currentInstruction->InstructionData.value().AddressingMode;

    if (currentAddressingMode == AddressingMode::Immediate || currentAddressingMode == AddressingMode::RegisterIndirectSource)
        _currentInstruction->InstructionData.value().MemoryOperand1 = get<uint8_t>(message.Data);
}

inline void ArithmeticLogicUnit::HandleMemoryResponseWriteBack(__attribute__((unused)) MemoryMessage message)
{
    return;
}

inline void ArithmeticLogicUnit::InitializeRegisters()
{
    _registers->Write(Register::IR, 0x00);
    _registers->WritePair(Register::PC, 0x0000);
    _registers->Write(Register::F, 0x00);
}

}