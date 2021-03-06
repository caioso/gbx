#pragma once

#define GL_SILENCE_DEPRECATION

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <iostream>

#include "GBXCoreExceptions.h"
#include "RAM.h"
#include "SystemConstants.h"
#include "VideoOutputInterface.h"

namespace gbxcore::video
{

class OpenGLVideoOutput : public gbxcore::interfaces::VideoOutputInterface
{
public:
    OpenGLVideoOutput(gbxcore::memory::RAM*);
    virtual ~OpenGLVideoOutput() = default;

    void Initialize();

    void Render() override;
    void SetVideoEnable(bool) override;
    void SetWindowEnable(bool) override;
    void SetBackgroundEnable(bool) override;
    void SetSpriteEnable(bool) override;
    void SetWindowTileMapBaseAddress(size_t) override;
    void SetBackgroundTileMapBaseAddress(size_t) override;
    void SetBackgroundAndWindowTileSetBaseAddress(size_t) override;
    void SetSpriteMode(gbxcore::interfaces::SpriteMode) override;
    void SetWindowScrolXY(size_t, size_t) override;
    void SetBackgroundScrolXY(size_t, size_t) override;
    void SetDMGBackgroundPalette(gbxcore::interfaces::OutputPalette) override;
    void SetDMGSpritePalette(uint8_t, gbxcore::interfaces::OutputPalette) override;
    void SetCGBBackgroundPalette(uint8_t, gbxcore::interfaces::OutputPalette) override;
    void SetCGBSpritePalette(uint8_t, gbxcore::interfaces::OutputPalette) override;

protected:
    inline void InitializeGLFW();
    inline void InitializeOpenGL();
    inline void InitializeRendererThread();

    void InitializeTexture();
    void RenderFrame();
    void ClearFrame();
    void FillOpenGLBuffer();
    void BindTexture();
    void RenderTexture();
    void ConvertTileToPixel();

    GLFWwindow* _window;
    gbxcore::memory::RAM* _dmgbcVideoRAM;

    float _viewPortScalingFactorX;
    float _viewPortScalingFactorY;

    bool _loopEnded{};
    bool _terminated{};

    // Open GL-related member atributes
    GLubyte _openGLViewportBuffer[gbxcore::constants::ScreenViewportBufferSizeInBytes];
    GLuint _texture;

    // The size of the frame pixels needs to be larger. Only part of it will be coppied to the OpenGL Viewport Buffer.
    gbxcore::interfaces::RGBColor _gbxFramePixels[gbxcore::constants::DMGBCMaxBackgroundHorizontalTileCount * gbxcore::constants::DMGBCMaxBackgroundVerticalTileCount * 64];

    size_t _backgroundAndWindowTileSetBase{};
    size_t _backgroundTileMapBase{};
};

}