#pragma once

#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "GBXCoreExceptions.h"

namespace gbxcore::video
{

class OpenGLVideoOutput
{
public:
    OpenGLVideoOutput();
    ~OpenGLVideoOutput() = default;

    void Initialize();

private:
    GLFWwindow* _window;
};

}