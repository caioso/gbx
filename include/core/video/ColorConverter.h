#pragma once

#include <sstream>

#include "GBXCoreExceptions.h"
#include "VideoOutputInterface.h"

namespace gbxcore::video
{

class ColorConverter
{
public:
    static inline gbxcore::interfaces::RGBColor DMGColorToRGB(uint8_t colorIndex)
    {
        if (colorIndex == 0x00)
            return {.Red = 155, .Green = 188, .Blue = 15 };
        if (colorIndex == 0x01)
            return {.Red = 139, .Green = 172, .Blue = 15 };
        if (colorIndex == 0x02)
            return {.Red = 48, .Green = 98, .Blue = 48 };
        if (colorIndex == 0x03)
            return {.Red = 15, .Green = 56, .Blue = 16 };

        std::stringstream ss;
        ss << "Invalid DMG Color '0x" << std::hex << colorIndex << "'";
        throw gbxcore::VideoOutputException(ss.str());       
    }
};

}