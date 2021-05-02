#pragma once

#include <cstdlib>
#include <string>

namespace gbxcore::constants
{

// Memory Constants
// GBX Default *System* RAM and ROM size is 32KB
const size_t GBXSystemROMInitialAddress = 0x0000;
const size_t GBXSystemROMPhysicalSize = 0x8000;
constexpr size_t GBXSystemROMFinalAddress = GBXSystemROMInitialAddress + GBXSystemROMPhysicalSize;

const size_t GBXSystemRAMInitialAddress = 0x8000;
const size_t GBXSystemRAMPhysicalSize = 0x8000;
constexpr size_t GBXSystemRAMFinalAddress = GBXSystemRAMInitialAddress + GBXSystemRAMPhysicalSize;

// GB & GBC Default ROM bank size of 16KB
const size_t DMGBCROMBankSize = 0x4000;
const size_t FixedBankROMSize = 0x8000; // System Bootup code (255 bytes used of a 16KB ROM Bank) + Cartridge Fixed Bank 16KB

// Temporary (the dynamic Bank ROM size must change depending on the rom MBC. Also, gbx will have other configuration)
const size_t DMGBCMaxDynamicROMSize = 0x1FC000; // Max Dynamic Bank ROM Size 2MB - 16KB

const size_t DMGBCFixedROMInitialAddress = 0x0000;
const size_t UserFixedROMPhysicalSize = 0x4000;
constexpr size_t DMGBCFixedROMFinalAddress = DMGBCFixedROMInitialAddress + UserFixedROMPhysicalSize;

const size_t DMGBCBankedROMInitialAddress = 0x4000;
const size_t UserBankedROMPhysicalSize = 0x4000;
constexpr size_t DMGBCBankedROMFinalAddress = DMGBCBankedROMInitialAddress + UserBankedROMPhysicalSize;

const size_t DMGBCVideoRAMInitialAddress = 0x8000;
const size_t DMGBCVideoRAMPhysicalSize = 0x2000;
constexpr size_t DMGBCVideoRAMFinalAddress = DMGBCVideoRAMInitialAddress + DMGBCVideoRAMPhysicalSize;

const size_t DMGBCExternalRAMInitialAddress = 0xA000;
const size_t DMGBCExternalRAMPhysicalSize = 0x2000;
constexpr size_t DMGBCExternalRAMFinalAddress = DMGBCExternalRAMInitialAddress + DMGBCExternalRAMPhysicalSize;

const size_t DMGBCSystemRAMBank0InitialAddress = 0xC000;
const size_t DMGBCSystemRAMBank0PhysicalSize = 0x1000;
constexpr size_t DMGBCSystemRAMBank0FinalAddress = DMGBCSystemRAMBank0InitialAddress + DMGBCSystemRAMBank0PhysicalSize;

const size_t DMGBCSystemRAMBank1InitialAddress = 0xD000;
const size_t DMGBCSystemRAMBank1PhysicalSize = 0x1000;
constexpr size_t DMGBCSystemRAMBank1FinalAddress = DMGBCSystemRAMBank1InitialAddress + DMGBCSystemRAMBank1PhysicalSize;

const size_t DMGBCMirrorRAMInitialAddress = 0xE000;
const size_t DMGBCMirrorRAMPhysicalSize = 0x1E00;
constexpr size_t DMGBCMirrorRAMFinalAddress = DMGBCMirrorRAMInitialAddress + DMGBCMirrorRAMPhysicalSize;

const size_t DMGBCIORAMInitialAddress = 0xFF00;
const size_t DMGBCIORAMPhysicalSize = 0x80;
constexpr size_t DMGBCIORAMFinalAddress = DMGBCIORAMInitialAddress + DMGBCIORAMPhysicalSize;

const size_t DMGBCHRAMInitialAddress = 0xFF80;
const size_t DMGBCHRAMPhysicalSize = 0x7F;
constexpr size_t DMGBCHRAMFinalAddress = DMGBCHRAMInitialAddress + DMGBCHRAMPhysicalSize;

// Clock Constants
static const uint64_t GBCClockPeriod = 119;

// Video Constants
const float DefaultViewPortScaleX = 1.0f;
const float DefaultViewPortScaleY = 1.0f;

// Curerntly I'm experimenting with GB rendering, so the screen size will be set to 160x144
const size_t ScreenWidth = 256;
const size_t ScreenHeight = 144;
const size_t DMGBCScreenWidth = 160;
const size_t BufferSwapInterval = 1;

const size_t DMGBCBackgroundBufferSize = 0x0800;
const size_t DMGBCMaxBackgroundHorizontalTileCount = 32;
const size_t DMGBCMaxBackgroundVerticalTileCount = 32;

constexpr size_t ScreenViewportBufferSizeInBytes = ScreenWidth * ScreenHeight * 3;
constexpr size_t GBXViewportBufferSizeInBytes = ScreenWidth * ScreenHeight;

const std::string EmulatorWindowName = "GBX";

}