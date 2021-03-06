#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <memory>

#include "../src/runtime/DebugServer.h"
#include "../src/interfaces/ServerTransport.h"

using namespace std;
using namespace gbx;
using namespace gbx::runtime;
using namespace gbx::interfaces;

TEST(TestDebugServer, Construction) 
{
    auto debugServer = make_shared<DebugServer>();
}