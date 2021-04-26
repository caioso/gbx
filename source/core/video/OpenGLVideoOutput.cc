#include "OpenGLVideoOutput.h"

using namespace gbxcore;

namespace gbxcore::video
{

OpenGLVideoOutput::OpenGLVideoOutput()
{}

void OpenGLVideoOutput::Initialize()
{
    if(!glfwInit())
		throw VideoOutputException ("Failed to initialize GLFW");
}

}