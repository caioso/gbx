#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "test_utils/TestUtils.h"

int main(int argc, char* argv[])
{
    testing::AddGlobalTestEnvironment(new GBXTestEnvironment(argv[1]));

    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}