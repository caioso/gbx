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
    auto cpu = make_shared<CPUWrapperForTests>();
    cpu->Initialize();
    auto registerBank = cpu->GetRegisterBank();

    registerBank.Write(Register::A, 0x40);
    auto accumulator = registerBank.Read(Register::A);

    EXPECT_EQ(0x40, accumulator);
}

TEST(CPUTests, PCInitialization)
{
    auto cpu = make_shared<CPUWrapperForTests>();
    cpu->Initialize();

    auto pcValue = cpu->GetRegisterBank().ReadPair(Register::PC);
    EXPECT_EQ(0x0000, pcValue);
}