#include "OpenGLVideoOutput.h"

using namespace gbxcore;
using namespace gbxcore::constants;
using namespace gbxcore::interfaces;
using namespace std;

namespace gbxcore::video
{

OpenGLVideoOutput::OpenGLVideoOutput(gbxcore::memory::RAM* vram)
  : _dmgbcVideoRAM(vram)
  , _viewPortScalingFactorX(DefaultViewPortScaleX)
  , _viewPortScalingFactorY(DefaultViewPortScaleY)
{}

void OpenGLVideoOutput::Initialize()
{
    // Basic FLGW initialization routine. Will change later.
    InitializeGLFW();
}

inline void OpenGLVideoOutput::InitializeGLFW()
{
    this->InitializeRendererThread();
}

void OpenGLVideoOutput::InitializeRendererThread()
{
    if(!glfwInit())
        throw VideoOutputException ("Failed to initialize GLFW");
    
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    _window = glfwCreateWindow(ScreenWidth, ScreenHeight, EmulatorWindowName.c_str(), NULL, NULL);

    if (!_window)
    {
        glfwTerminate();
        throw VideoOutputException("Failed to open GLFW window");
    }

    glfwMakeContextCurrent(_window);
    glfwSetWindowSize(_window, ScreenWidth * _viewPortScalingFactorX, ScreenHeight * _viewPortScalingFactorY);
	glfwSwapInterval(static_cast<int>(BufferSwapInterval));

    InitializeOpenGL();   
}

inline void OpenGLVideoOutput::InitializeOpenGL()
{
    glViewport(0, 0, ScreenWidth * _viewPortScalingFactorX, ScreenHeight * _viewPortScalingFactorY);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, ScreenWidth * _viewPortScalingFactorX, 0.0, ScreenHeight * _viewPortScalingFactorY, 0.0, 1.0); // this creates a canvas you can do 2D drawing on
}

void OpenGLVideoOutput::Render()
{
    if (_terminated)
        return;
        
    // Terminate GLFW
    if(_loopEnded || glfwWindowShouldClose(_window))
    {
        glfwTerminate();
        _terminated = true;
    }
    else
	{
		// Draw gears

		// Swap buffers
		glfwSwapBuffers(_window);
		
        // WARNING glfwPoolEvents can only be called from the main thread
        // Move this to the CPU's main
        glfwPollEvents();
	}
}

void OpenGLVideoOutput::SetVideoEnable(bool)
{

}
void OpenGLVideoOutput::SetWindowEnable(bool)
{

}
void OpenGLVideoOutput::SetBackgroundEnable(bool)
{

}
void OpenGLVideoOutput::SetSpriteEnable(bool)
{

}
void OpenGLVideoOutput::SetWindowTileMapBaseAddress(size_t)
{

}
void OpenGLVideoOutput::SetBackgroundTileMapBaseAddress(size_t)
{

}
void OpenGLVideoOutput::SetBackgroundAndWindowTileSetBaseAddress(size_t)
{

}
void OpenGLVideoOutput::SetSpriteMode(SpriteMode)
{

}
void OpenGLVideoOutput::SetWindowScrolXY(size_t, size_t)
{

}
void OpenGLVideoOutput::SetBackgroundScrolXY(size_t, size_t)
{

}
void OpenGLVideoOutput::SetDMGBackgroundPalette(OutputPalette)
{

}
void OpenGLVideoOutput::SetDMGSpritePalette(uint8_t, OutputPalette)
{

}
void OpenGLVideoOutput::SetCGBBackgroundPalette(uint8_t, OutputPalette)
{

}
void OpenGLVideoOutput::SetCGBSpritePalette(uint8_t, OutputPalette)
{

}

}