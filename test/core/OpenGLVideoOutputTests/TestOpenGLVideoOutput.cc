#include <gtest/gtest.h>

#include "TestUtils.h"
#include "CoreTestMocksAndWrappers.h"

#include <array>
#include <cstdlib>
#include <memory>
#include <optional>
#include <variant>

#include "OpenGLVideoOutput.h"
#include "VideoOutputInterface.h"

using namespace std; 
using namespace gbxcore::interfaces;
using namespace gbxcore::video;

// GLFW/OpenGL test doubles
bool glfwInitialized;

int glfwInit(void)
{
    glfwInitialized = true;
    return 1;
}


TEST(CoreTests_OpenGLVideoOutput, OpenGLVideoOutputConstruction) 
{
    OpenGLVideoOutput output;
}

TEST(CoreTests_OpenGLVideoOutput, InitializeVideoOutput) 
{
    // Initialize Test Checks
    glfwInitialized = false;

    OpenGLVideoOutput output;
    output.Initialize();

    EXPECT_TRUE(glfwInitialized);
}