#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "CancellationToken.h"

using namespace std;
using namespace gbxruntime;
using namespace gbxruntime::runner;

TEST(RuntimeTests_CancellationToken, Construction) 
{
    CancellationToken token;
    EXPECT_FALSE(token.IsCancelled());
}

TEST(RuntimeTests_CancellationToken, CancelToken) 
{
     CancellationToken token;
     token.Cancel();
     EXPECT_TRUE(token.IsCancelled());
}
