#pragma once

#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include "GBXCoreExceptions.h"
#include "RAM.h"
#include "SystemConstants.h"

namespace gbxcore::video
{

class OpenGLVideoOutput
{
public:
    OpenGLVideoOutput(gbxcore::memory::RAM*);
    ~OpenGLVideoOutput() = default;

    void Initialize();

private:
    inline void CreateWindow();

    GLFWwindow* _window;
    gbxcore::memory::RAM* _dmgbcVideoRAM;

    float _viewPortScalingFactorX;
    float _viewPortScalingFactorY;
};

}