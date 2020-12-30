#include <gtest/gtest.h>

#include <algorithm>
#include <memory>
#include <optional>

#include "../src/Channel.h"
#include "../src/MemoryController.h"
#include "../src/RegisterBank.h"
#include "../src/instructions/LD.h"

using namespace std;
using namespace gbx;

uint8_t BinaryRegisterAddressingMode(Register source, Register destination)
{
    return RegisterBank::ToInstructionSource(source) | 
           RegisterBank::ToInstructionDestination(destination) << 3 |
           0x01 << 6;
}

uint8_t BinaryImmediateAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBank::ToInstructionDestination(destination) << 3 |
           0x00 << 6;
}

uint8_t BinaryRegisterIndirectAsSourceAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBank::ToInstructionDestination(destination) << 3 |
           0x01 << 6;
}

uint8_t BinaryRegisterIndirectAsDestinationAddressingMode(Register source)
{
    return RegisterBank::ToInstructionSource(source) | 
           0x06 << 3 |
           0x01 << 6;
}

uint8_t BinaryRegisterIndexedAddressingMode(Register destination)
{
    return 0x06 | 
           RegisterBank::ToInstructionDestination(destination) << 3 | 
           0x01 << 6;
}

TEST(TestLD, ExecuteUndecodedInstruction)
{
    auto registerBank = make_shared<RegisterBank>();
    auto memoryChannel = make_shared<Channel<MemoryMessage>>();
    auto dummyChannelEnd = make_shared<Channel<MemoryMessage>>();

    memoryChannel->Bind(dummyChannelEnd);
    dummyChannelEnd->Bind(memoryChannel);

    memoryChannel->OnReceived([](MemoryMessage) -> void {});
    dummyChannelEnd->OnReceived([](MemoryMessage)-> void {});

    auto testPassed = false;

    try
    {
        LD ld;
        ld.Execute(registerBank, memoryChannel);
    }
    catch (const InstructionException& e)
    {
        testPassed = true;
    }

    EXPECT_TRUE(testPassed);
}
   

TEST(TestLD, DecodeImmediateAddressingMode)
{
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto destination : destinationsList)
    {
        LD ld;
        auto rawBinary = BinaryImmediateAddressingMode(destination);
        ld.Decode(rawBinary, nullopt);

        EXPECT_NE(nullopt, ld.InstructionData);
        EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
        EXPECT_EQ(AddressingMode::Immediate, ld.InstructionData.value().AddressingMode);
        EXPECT_EQ(Register::NoRegiser, ld.InstructionData.value().SourceRegister);
        EXPECT_EQ(destination, ld.InstructionData.value().DestinationRegister);
    }

    const auto opcode = 0x00;
    const auto suffix = 0x06;
    constexpr auto forbiddenDestinationRegister = (0x06 << 3);
    auto forbiddenDestinationTestPassed = false;
    auto incorrectDestinationBinary = suffix | forbiddenDestinationRegister | opcode;

    try
    {
        LD ld;
        ld.Decode(incorrectDestinationBinary, nullopt);
    }
    catch (const RegisterBankException& e)
    {
        forbiddenDestinationTestPassed = true;
    }

    EXPECT_TRUE(forbiddenDestinationTestPassed);
}

TEST(TestLD, DecodingRegisterAddressingMode)
{
    auto sourcesList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::F, Register::L};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};

    for (auto source : sourcesList)
    for (auto destination : destinationsList)
    {
        LD ld;
        auto rawBinary = BinaryRegisterAddressingMode(source, destination);
        ld.Decode(rawBinary, nullopt);

        EXPECT_NE(nullopt, ld.InstructionData);
        EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
        EXPECT_EQ(AddressingMode::Register, ld.InstructionData.value().AddressingMode);
        EXPECT_EQ(source, ld.InstructionData.value().SourceRegister);
        EXPECT_EQ(destination, ld.InstructionData.value().DestinationRegister);
    }

    auto invalidDestination = {Register::F};
    auto invalidSource = {Register::H};

    for (auto source : sourcesList)
    for (auto destination : invalidDestination)
    {
        auto testPassed = false;

        try
        {
            LD ld;
            auto rawBinary = BinaryRegisterAddressingMode(source, destination);
            ld.Decode(rawBinary, nullopt);
        }
        catch (const RegisterBankException& e)
        {
            testPassed = true;
        }

        EXPECT_TRUE(testPassed);
    }

    for (auto destination : destinationsList)
    for (auto source : invalidSource)
    {
        auto testPassed = false;

        try
        {
            LD ld;
            auto rawBinary = BinaryRegisterAddressingMode(source, destination);
            ld.Decode(rawBinary, nullopt);
        }
        catch (const RegisterBankException& e)
        {
            testPassed = true;
        }

        EXPECT_TRUE(testPassed);
    }

    for (auto destination : destinationsList)
    {
        LD ld;
        constexpr auto opcode = 0x01 << 6;
        constexpr auto forbiddenSourceRegister = (0x06);
        auto bin = forbiddenSourceRegister | RegisterBank::ToInstructionDestination(destination) << 3 | opcode;

        ld.Decode(bin, nullopt);
        // using 0x06 as source will lead to register indirect (thus using (HL));
        EXPECT_EQ(AddressingMode::RegisterIndirectSource, ld.InstructionData.value().AddressingMode);
    }

    for (auto source : sourcesList)
    {
        LD ld;
        constexpr auto opcode = 0x01 << 6;
        constexpr auto forbiddenDestinationRegister = (0x06 << 3);
        auto bin = RegisterBank::ToInstructionSource(source) | forbiddenDestinationRegister | opcode;

        ld.Decode(bin, nullopt);
        
        EXPECT_EQ(AddressingMode::RegisterIndirectDestination, ld.InstructionData.value().AddressingMode);
    }
}

TEST(TestLD, DecodeRegisterIndirectAddressingMode)
{
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto rawBinary = static_cast<uint8_t>(0x00);
    LD ld;

    for (auto destination : destinationsList)
    {
        rawBinary = BinaryRegisterIndirectAsSourceAddressingMode(destination);
        ld.Decode(rawBinary, nullopt);

        EXPECT_NE(nullopt, ld.InstructionData);
        EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectSource, ld.InstructionData.value().AddressingMode);
        EXPECT_EQ(Register::HL, ld.InstructionData.value().SourceRegister);
        EXPECT_EQ(destination, ld.InstructionData.value().DestinationRegister);
    }

    // (BC) and (DE) as sources -> Only A is accepted as source
    rawBinary = 0x0A; // Ld A, (BC)
    ld.Decode(rawBinary, nullopt);

    EXPECT_NE(nullopt, ld.InstructionData);
    EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, ld.InstructionData.value().AddressingMode);
    EXPECT_EQ(Register::BC, ld.InstructionData.value().SourceRegister);
    EXPECT_EQ(Register::A, ld.InstructionData.value().DestinationRegister);

    rawBinary = 0x1A; // Ld A, (DE)
    ld.Decode(rawBinary, nullopt);

    EXPECT_NE(nullopt, ld.InstructionData);
    EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectSource, ld.InstructionData.value().AddressingMode);
    EXPECT_EQ(Register::DE, ld.InstructionData.value().SourceRegister);
    EXPECT_EQ(Register::A, ld.InstructionData.value().DestinationRegister);

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::F, Register::L};

    for (auto source : sourceList)
    {
        rawBinary = BinaryRegisterIndirectAsDestinationAddressingMode(source);
        ld.Decode(rawBinary, nullopt);

        EXPECT_NE(nullopt, ld.InstructionData);
        EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndirectDestination, ld.InstructionData.value().AddressingMode);
        EXPECT_EQ(source, ld.InstructionData.value().SourceRegister);
        EXPECT_EQ(Register::HL, ld.InstructionData.value().DestinationRegister);
    }

    // (BC) and (DE) as sources -> Only A is accepted as source
    rawBinary = 0x02; // Ld (BC), A
    ld.Decode(rawBinary, nullopt);

    EXPECT_NE(nullopt, ld.InstructionData);
    EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestination, ld.InstructionData.value().AddressingMode);
    EXPECT_EQ(Register::A, ld.InstructionData.value().SourceRegister);
    EXPECT_EQ(Register::BC, ld.InstructionData.value().DestinationRegister);

    rawBinary = 0x12; // Ld (DE), A
    ld.Decode(rawBinary, nullopt);

    EXPECT_NE(nullopt, ld.InstructionData);
    EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
    EXPECT_EQ(AddressingMode::RegisterIndirectDestination, ld.InstructionData.value().AddressingMode);
    EXPECT_EQ(Register::A, ld.InstructionData.value().SourceRegister);
    EXPECT_EQ(Register::DE, ld.InstructionData.value().DestinationRegister);

    const auto suffix = 0x06;
    constexpr auto opcode = 0x01 << 6;
    constexpr auto forbiddenDestinationRegister = (0x06 << 3);
    auto forbiddenDestinationTestPassed = false;
    auto incorrectDestinationBinary = suffix | forbiddenDestinationRegister | opcode;

    try
    {
        LD ld;
        ld.Decode(incorrectDestinationBinary, nullopt);
    }
    catch (const RegisterBankException& e)
    {
        forbiddenDestinationTestPassed = true;
    }

    EXPECT_TRUE(forbiddenDestinationTestPassed);
}

TEST(TestLD, DecodeRegisterIndexedAddressingMode)
{
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    auto preOpcode = static_cast<uint8_t>(0xDD);
    LD ld;

    for (auto destination : destinationsList)
    {
        auto rawBinary = BinaryRegisterIndexedAddressingMode(destination);
        ld.Decode(rawBinary, preOpcode);

        EXPECT_NE(nullopt, ld.InstructionData);
        EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndexedSource, ld.InstructionData.value().AddressingMode);
        EXPECT_EQ(Register::IX, ld.InstructionData.value().SourceRegister);
        EXPECT_EQ(destination, ld.InstructionData.value().DestinationRegister);
    }

    preOpcode = static_cast<uint8_t>(0xFD);

    for (auto destination : destinationsList)
    {
        auto rawBinary = BinaryRegisterIndexedAddressingMode(destination);
        ld.Decode(rawBinary, preOpcode);

        EXPECT_NE(nullopt, ld.InstructionData);
        EXPECT_EQ(OpcodeType::ld, ld.InstructionData.value().Opcode);
        EXPECT_EQ(AddressingMode::RegisterIndexedSource, ld.InstructionData.value().AddressingMode);
        EXPECT_EQ(Register::IY, ld.InstructionData.value().SourceRegister);
        EXPECT_EQ(destination, ld.InstructionData.value().DestinationRegister);
    }
}

TEST(TestLD, ExecuteImmediateAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto memoryChannel = make_shared<Channel<MemoryMessage>>();
    auto dummyChannelEnd = make_shared<Channel<MemoryMessage>>();
    auto memoryContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    dummyChannelEnd->OnReceived([](MemoryMessage)-> void {});
    memoryChannel->Bind(dummyChannelEnd);
    dummyChannelEnd->Bind(memoryChannel);
    memoryChannel->OnReceived([](MemoryMessage)-> void {});

    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < destinationsList.size(); i++)
    {
        LD ld;
        auto rawBinary = BinaryImmediateAddressingMode(*(begin(destinationsList) + i));
        ld.Decode(rawBinary, nullopt);

        // simulate ALU acquiring operand from memory.
        ld.InstructionData.value().MemoryOperand1 = *(begin(memoryContent) + i);
        
        ld.Execute(registerBank, memoryChannel);

        EXPECT_EQ(*(begin(memoryContent) + i), registerBank->Read(*(begin(destinationsList) + i)));
    }
}

TEST(TestLD, ExecuteRegisterAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto memoryChannel = make_shared<Channel<MemoryMessage>>();
    auto dummyChannelEnd = make_shared<Channel<MemoryMessage>>();
    auto registersContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    dummyChannelEnd->OnReceived([](MemoryMessage)-> void {});
    memoryChannel->Bind(dummyChannelEnd);
    dummyChannelEnd->Bind(memoryChannel);
    memoryChannel->OnReceived([](MemoryMessage)-> void {});

    auto sourcesList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::F, Register::L};
    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    auto registerCounter = 0;
    for (auto source : sourcesList)
    {
        auto sourceValue = *(begin(registersContent) + (registerCounter++));
        registerBank->Write(source, sourceValue);
        
        for (auto destination : destinationsList)
            if (destination != source)
                registerBank->Write(destination, 0x00);

        for (auto destination : destinationsList)
        {
            LD ld;
            auto rawBinary = BinaryRegisterAddressingMode(source, destination);
            ld.Decode(rawBinary, nullopt);
            ld.Execute(registerBank, memoryChannel);

            EXPECT_EQ(sourceValue, registerBank->Read(destination));
        }

        EXPECT_EQ(sourceValue, registerBank->Read(source));
    }
}

TEST(TestLD, ExecuteRegisterIndirectSourceAddressingMode)
{
   auto registerBank = make_shared<RegisterBank>();
    auto memoryChannel = make_shared<Channel<MemoryMessage>>();
    auto dummyChannelEnd = make_shared<Channel<MemoryMessage>>();
    auto memoryContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    dummyChannelEnd->OnReceived([](MemoryMessage)-> void {});
    memoryChannel->Bind(dummyChannelEnd);
    dummyChannelEnd->Bind(memoryChannel);
    memoryChannel->OnReceived([](MemoryMessage)-> void {});

    auto destinationsList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::H, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < destinationsList.size(); i++)
    {
        LD ld;
        auto rawBinary = BinaryRegisterIndirectAsSourceAddressingMode(*(begin(destinationsList) + i));
        ld.Decode(rawBinary, nullopt);

        // simulate ALU acquiring operand from memory.
        ld.InstructionData.value().MemoryOperand1 = *(begin(memoryContent) + i);
        
        ld.Execute(registerBank, memoryChannel);

        EXPECT_EQ(*(begin(memoryContent) + i), registerBank->Read(*(begin(destinationsList) + i)));
    }
}

TEST(TestLD, ExecuteRegisterIndirectDestinationAddressingMode)
{
    auto registerBank = make_shared<RegisterBank>();
    auto memoryChannel = make_shared<Channel<MemoryMessage>>();
    auto dummyChannelEnd = make_shared<Channel<MemoryMessage>>();
    auto registerContent = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x11};

    dummyChannelEnd->OnReceived([](MemoryMessage)-> void {});
    memoryChannel->Bind(dummyChannelEnd);
    dummyChannelEnd->Bind(memoryChannel);
    memoryChannel->OnReceived([](MemoryMessage)-> void {});

    auto sourceList = {Register::A, Register::B, Register::C, Register::D, Register::E, Register::F, Register::L};
    
    for (auto i = static_cast<size_t>(0); i < sourceList.size(); i++)
    {
        LD ld;
        auto rawBinary = BinaryRegisterIndirectAsDestinationAddressingMode(*(begin(sourceList) + i));
        ld.Decode(rawBinary, nullopt);

        registerBank->Write(*(begin(sourceList) + i), *(begin(registerContent) + i));
        ld.Execute(registerBank, memoryChannel);

        EXPECT_EQ(*(begin(registerContent) + i), ld.InstructionData.value().MemoryResult1);
    }
}

TEST(TestLD, ExecuteRegisterIndexSourceAddressingMode)
{
    
}