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
    InitializeTexture();  
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
		// Render Frame
        RenderFrame();

		// Swap buffers
		glfwSwapBuffers(_window);
		
        // WARNING glfwPoolEvents can only be called from the main thread
        // Move this to the CPU's main
        glfwPollEvents();
	}
}

void OpenGLVideoOutput::InitializeTexture()
{
    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // This only works in DMGBC mode for now
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gbxcore::constants::DMGBCScreenWidth, gbxcore::constants::ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,  _openGLViewportBuffer);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLVideoOutput::RenderFrame()
{
    //ClearFrame();
    ConvertTileToPixel();
    FillOpenGLBuffer();
    BindTexture();
    RenderTexture();
}

void OpenGLVideoOutput::BindTexture()
{
    glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _texture);
    // This only works in DMGBC mode for now
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, gbxcore::constants::DMGBCScreenWidth, gbxcore::constants::ScreenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE,  _openGLViewportBuffer);
}

void OpenGLVideoOutput::RenderTexture()
{
    glBindTexture(GL_TEXTURE_2D, _texture);
     // This only works in DMGBC mode for now
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); 
            glVertex3f(((gbxcore::constants::ScreenWidth - gbxcore::constants::DMGBCScreenWidth)*_viewPortScalingFactorX)/2, 0, 0);
        glTexCoord2f(0, -1); 
            glVertex3f(((gbxcore::constants::ScreenWidth - gbxcore::constants::DMGBCScreenWidth)*_viewPortScalingFactorX)/2, 
                         gbxcore::constants::ScreenHeight*_viewPortScalingFactorY, 0);
        glTexCoord2f(1, -1); 
            glVertex3f((gbxcore::constants::DMGBCScreenWidth*_viewPortScalingFactorX) + 
                      ((gbxcore::constants::ScreenWidth - gbxcore::constants::DMGBCScreenWidth)*_viewPortScalingFactorX)/2, 
                        gbxcore::constants::ScreenHeight*_viewPortScalingFactorY, 0);
        glTexCoord2f(1, 0); 
            glVertex3f((gbxcore::constants::DMGBCScreenWidth*_viewPortScalingFactorX) + 
                      ((gbxcore::constants::ScreenWidth - gbxcore::constants::DMGBCScreenWidth)*_viewPortScalingFactorX)/2, 0, 0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void OpenGLVideoOutput::ConvertTileToPixel()
{
    size_t numberOfPixels = 0;
    // What to do:
    // 1: Pick the tile index from the BG & Window Tile Set
    // 2: Convert the pixels from that tile to RGB
    //    Note that tiles are 8 x 8.
    //    There needs to be direct access to the video RAM raw memory bytes!!!!
    // 3: Consider windowing of the background and the viewport.
    for (auto j = 0llu; j < gbxcore::constants::DMGBCMaxBackgroundHorizontalTileCount; ++j)
        for (auto i = 0llu; i < gbxcore::constants::DMGBCMaxBackgroundHorizontalTileCount; ++i) // This goes up to the number of TILES on screen
        {
            // tile
            // Adjust tile position based on the screen 'windowing'
            auto tile = get<uint8_t>(_dmgbcVideoRAM->Read(_backgroundTileMapBase + (i + j*gbxcore::constants::DMGBCMaxBackgroundHorizontalTileCount), MemoryAccessType::Byte));

            // Convert the tile's pixels
            auto tilePixelsBasePosition = _backgroundAndWindowTileSetBase + 16*tile;
            auto initialPixelAddressInVideoBuffer = i * 8 + j * gbxcore::constants::DMGBCMaxBackgroundHorizontalTileCount * 8 * 8;
            auto tileLineCounter = 0;

            for (auto k = 0; k < 16; k += 2)
            {
                auto msByte = get<uint8_t>(_dmgbcVideoRAM->Read(tilePixelsBasePosition + k, MemoryAccessType::Byte));
                auto lsByte = get<uint8_t>(_dmgbcVideoRAM->Read(tilePixelsBasePosition + k + 1, MemoryAccessType::Byte));

                for (auto l = 0; l < 8; ++l)
                {
                    _gbxFramePixels[initialPixelAddressInVideoBuffer + l + (tileLineCounter * gbxcore::constants::DMGBCMaxBackgroundHorizontalTileCount * 8)] = 
                        ByteToColor((((msByte >> (7 - l)) & 0x01) << 0x01) | ((lsByte >> (7 - l)) & 0x01));

                    numberOfPixels++;
                }
                tileLineCounter++;
            }
        }
}

void OpenGLVideoOutput::FillOpenGLBuffer()
{
    // The renderiong 
    constexpr auto limit = gbxcore::constants::ScreenHeight * gbxcore::constants::DMGBCScreenWidth;
    
    auto offset = 0llu;
    auto lineCounter = 0;
    for (auto pointer = 0llu; pointer < limit; ++pointer, ++lineCounter)
    {
        if (lineCounter == gbxcore::constants::DMGBCScreenWidth)
        {
            offset += (gbxcore::constants::DMGBCMaxBackgroundHorizontalTileCount - gbxcore::constants::DMGBCScreenWidth/8) *8;
            lineCounter = 0;
        }
        auto destination = _openGLViewportBuffer + (pointer)*3;
        auto sourceIndex = (pointer + offset);

        destination[0] = _gbxFramePixels[sourceIndex].Red;
        destination[1] = _gbxFramePixels[sourceIndex].Green;
        destination[2] = _gbxFramePixels[sourceIndex].Blue;
    }
}

void OpenGLVideoOutput::ClearFrame()
{
    constexpr auto limit = gbxcore::constants::ScreenHeight * gbxcore::constants::DMGBCScreenWidth;
    
    for (auto pointer = 0llu; pointer < limit; ++pointer)
        _gbxFramePixels[pointer] = gbxcore::interfaces::VideoOutputInterface::ByteToColor(0);
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
void OpenGLVideoOutput::SetBackgroundTileMapBaseAddress(size_t address)
{
    if (address == 0x9800)
        _backgroundTileMapBase = 0x1800;
    else 
        _backgroundTileMapBase = 0x1C00;
}
void OpenGLVideoOutput::SetBackgroundAndWindowTileSetBaseAddress(size_t address)
{
    if (address == 0x8000)
        _backgroundAndWindowTileSetBase = 0x0000;
    else
        _backgroundAndWindowTileSetBase = 0x0800;
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