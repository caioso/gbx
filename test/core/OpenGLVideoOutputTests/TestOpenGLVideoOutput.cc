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

struct GLFWwindow
{
    int value;
};
GLFWwindow window;

typedef uint32_t GLbitfield;
typedef uint8_t  GLboolean;
typedef int8_t   GLbyte;
typedef float    GLclampf;
typedef uint32_t GLenum;
typedef float    GLfloat;
typedef int32_t  GLint;
typedef int16_t  GLshort;
typedef int32_t  GLsizei;
typedef uint8_t  GLubyte;
typedef uint32_t GLuint;
typedef uint16_t GLushort;
typedef void     GLvoid;

int glfwInit(void)
{
    glfwInitialized = true;
    return 1;
}

void glfwWindowHint(int, int)
{}

GLFWwindow* glfwCreateWindow(int, int, const char*, GLFWmonitor*, GLFWwindow*)
{
    return reinterpret_cast<GLFWwindow*>(&window);
}

void glfwTerminate(void)
{}

void glfwMakeContextCurrent(GLFWwindow*)
{}

void glfwSetWindowSize(GLFWwindow*, int, int)
{}

void glfwSwapInterval(int)

{}
void glfwSwapBuffers(GLFWwindow*)
{}

void glfwPollEvents(void)
{}

int glfwWindowShouldClose(GLFWwindow*)
{
    return 1;
}

void glViewport (GLint, GLint, GLsizei, GLsizei)
{}

void glMatrixMode (GLenum)
{}

void glLoadIdentity (void)
{}

void glOrtho (GLdouble, GLdouble, GLdouble, GLdouble, GLdouble, GLdouble)
{}

void glBindTexture (GLenum, GLuint)
{}

void glGenTextures (GLsizei, GLuint*)
{}

void glTexImage2D (GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const GLvoid *)
{}

void glTexParameteri (GLenum, GLenum, GLint)
{}

void glBegin (GLenum)
{}

void glDisable(GLenum)
{}

void glEnable (GLenum)
{}

void glEnd (void)
{}

void glTexCoord2f (GLfloat, GLfloat)
{}

void glVertex3f (GLfloat, GLfloat, GLfloat)
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