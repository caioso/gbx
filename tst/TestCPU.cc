#include <gtest/gtest.h>
#include <array>

#include "../src/CPU.h"
#include "../src/RegisterBank.h"

namespace gbx
{
class CPUWrapperForTests : public CPU
{
public:
     RegisterBank& GetRegisterBank()
     {
         return *_registerBank;
     }
};
}

using namespace std;
using namespace gbx;

TEST(CPUTests, Construction) 
{
    CPUWrapperForTests cpu;
    auto registerBank = cpu.GetRegisterBank();

    registerBank.Write(Register::A, 0x40);
    auto accumulator = registerBank.Read(Register::A);

    EXPECT_EQ(0x40, accumulator);
}

TEST(CPUTests, PCInitialization)
{
    CPUWrapperForTests cpu;
    auto pcValue = cpu.GetRegisterBank().ReadPair(Register::PC);
    EXPECT_EQ(0x0000, pcValue);
}

TEST(CPUTests, MachineCyclePCUpdate)
{
    CPUWrapperForTests cpu;
    auto pcValue = cpu.GetRegisterBank().ReadPair(Register::PC);
    EXPECT_EQ(0x0000, pcValue);

    cpu.Run();

    pcValue = cpu.GetRegisterBank().ReadPair(Register::PC);
    EXPECT_EQ(0x0001, pcValue);
}

TEST(CPUTests, MultipleMachineCylesPCUpdate)
{
    CPUWrapperForTests cpu;

    for (auto i = 0; i < 100; i++)
        cpu.Run();

    auto pcFinalValue = cpu.GetRegisterBank().ReadPair(Register::PC);
    EXPECT_EQ(100, pcFinalValue);
}