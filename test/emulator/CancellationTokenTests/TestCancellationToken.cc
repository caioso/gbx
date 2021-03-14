#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "CancellationToken.h"

using namespace std;
using namespace gbx;
using namespace gbx::runtime;

TEST(TestCancellationToken, Construction) 
{
    CancellationToken token;
    EXPECT_FALSE(token.IsCancelled());
}

TEST(TestCancellationToken, CancelToken) 
{
     CancellationToken token;
     token.Cancel();
     EXPECT_TRUE(token.IsCancelled());
}
