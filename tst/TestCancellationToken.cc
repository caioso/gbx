#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "../src/CancellationToken.h"

using namespace std;
using namespace gbx;

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
