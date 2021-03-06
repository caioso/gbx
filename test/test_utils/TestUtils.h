#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <string>

#define ASSERT_EXCEPTION( TRY_BLOCK, EXCEPTION_TYPE, MESSAGE )        \
try                                                                   \
{                                                                     \
    TRY_BLOCK                                                         \
    FAIL() << "exception '" << MESSAGE << "' not thrown";             \
}                                                                     \
catch( const EXCEPTION_TYPE& e )                                      \
{                                                                     \
    EXPECT_STREQ( MESSAGE, e.what() )                                 \
        << " exception message is incorrect. Expected the following " \
           "message:\n\n"                                             \
        << e.what() << "\n";                                          \
}                                                                     \
catch( ... )                                                          \
{                                                                     \
    FAIL() << "exception '" << MESSAGE                                \
           << "' not thrown with expected type '" << #EXCEPTION_TYPE  \
           << "'!";                                                   \
}

class GBXTestEnvironment : public testing::Environment 
{
public:
    GBXTestEnvironment(std::string testDataPath)
    {
        TestDataPath = testDataPath;
    }

    virtual ~GBXTestEnvironment() {}
    virtual void SetUp() {}
    virtual void TearDown() {}

    inline static std::string TestDataPath{};
};