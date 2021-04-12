#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "CoreTestMocksAndWrappers.h"

#include <memory>

#include "MemoryController.h"
#include "MemoryMappedRegister.h"
#include "InterruptEnableRegister.h"

using namespace std;
using namespace gbxcore;
using namespace gbxcore::memory;
using namespace gbxcore::memory::registers;

TEST(CoreTests_MemoryMappedRegister, InterruptEnableRegisterConstruction) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
}

TEST(CoreTests_MemoryMappedRegister, RegisterInterruptEnableRegister) 
{
    auto interruptEnableRegister = make_unique<InterruptEnableRegister>();
    MemoryController controller;

    controller.RegisterMemoryMappedRegister(std::move(interruptEnableRegister), 0xFFFF, Ownership::System);
}
