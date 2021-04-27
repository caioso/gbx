#include <gtest/gtest.h>

#include "TestUtils.h"
#include "CoreTestMocksAndWrappers.h"

#include <array>
#include <cstdlib>
#include <memory>
#include <optional>
#include <variant>

#include "MemoryController.h"
#include "OpenGLVideoOutput.h"
#include "RAM.h"
#include "SystemConstants.h"
#include "VideoOutputInterface.h"

using namespace std; 
using namespace gbxcore::constants;
using namespace gbxcore::interfaces;
using namespace gbxcore::memory;
using namespace gbxcore::video;

// GLFW/OpenGL test doubles
bool glfwInitialized;

int glfwInit(void)
{
    glfwInitialized = true;
    return 1;
}

void glfwWindowHint(int, int)
{}

GLFWwindow* glfwCreateWindow(int, int, const char*, GLFWmonitor*, GLFWwindow*)
{}

void glfwTerminate(void)
{}

// Wrappers
class OpenGLVideoOutputWrapper : public OpenGLVideoOutput
{
public:
    OpenGLVideoOutputWrapper(gbxcore::memory::RAM* vram)
        : OpenGLVideoOutput(vram)
    {}

    virtual ~OpenGLVideoOutputWrapper() = default;
};

TEST(CoreTests_OpenGLVideoOutput, ConstructVideoOutput) 
{
    // Initialize Test Checks
    glfwInitialized = false;

    auto userVideoRAM = make_unique<RAM>(DMGBCVideoRAMPhysicalSize);
    OpenGLVideoOutput output(userVideoRAM.get());
}

TEST(CoreTests_OpenGLVideoOutput, TestWindowCreation) 
{
    // Initialize Test Checks
    glfwInitialized = false;

    auto userVideoRAM = make_unique<RAM>(DMGBCVideoRAMPhysicalSize);
    OpenGLVideoOutput output(userVideoRAM.get());
    output.Initialize();

    EXPECT_TRUE(glfwInitialized);
}