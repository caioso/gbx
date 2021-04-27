#include "OpenGLVideoOutput.h"

using namespace gbxcore;
using namespace gbxcore::constants;

namespace gbxcore::video
{

OpenGLVideoOutput::OpenGLVideoOutput(gbxcore::memory::RAM* vram)
  : _dmgbcVideoRAM(vram)
  , _viewPortScalingFactorX(DefaultViewPortScaleX)
  , _viewPortScalingFactorY(DefaultViewPortScaleY)
{}

void OpenGLVideoOutput::Initialize()
{
    if(!glfwInit())
		  throw VideoOutputException ("Failed to initialize GLFW");

    // Basic FLGW initialization routine. Will change later.
    CreateWindow();
}

inline void OpenGLVideoOutput::CreateWindow()
{
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    _window = glfwCreateWindow(ScreenWidth, ScreenHeight, EmulatorWindowName.c_str(), 0, 0);

    if (!_window)
    {
        glfwTerminate();
        throw VideoOutputException("Failed to open GLFW window");
    }
}

}