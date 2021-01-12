#include <gtest/gtest.h>
#include <gmock/gmock.h>

int main(__attribute__((unused)) int argc, __attribute__((unused)) char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv); 
    return RUN_ALL_TESTS();
}