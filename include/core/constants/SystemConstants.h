#pragma once

#include <cstdlib>

namespace gbxcore::constants
{

// GB & GBC Default ROM bank size of 16KB
const size_t DefaultROMBankSize = 0x4000;
const size_t FixedBankROMSize = 0x8000; // System Bootup code (255 bytes used of a 16KB ROM Bank) + Cartridge Fixed Bank 16KB

// MBC Confifg
const size_t MBC1DynamicBankROMSize = 0xC000; // 48KB in Bank + 16KB Fixed

const size_t UserFixedROMInitialAddress = 0x0000;
const size_t UserFixedROMPhysicalSize = 0x4000;
constexpr size_t UserFixedROMFinalAddress = UserFixedROMInitialAddress + UserFixedROMPhysicalSize;

const size_t UserBankedROMInitialAddress = 0x4000;
const size_t UserBankedROMPhysicalSize = 0x4000;
constexpr size_t UserBankedROMFinalAddress = UserBankedROMInitialAddress + UserBankedROMPhysicalSize;

const size_t VideoRAMInitialAddress = 0x8000;
const size_t VideoRAMPhysicalSize = 0x2000;
constexpr size_t VideoRAMFinalAddress = VideoRAMInitialAddress + VideoRAMPhysicalSize;

const size_t ExternalRAMInitialAddress = 0xA000;
const size_t ExternalRAMPhysicalSize = 0x2000;
constexpr size_t ExternalRAMFinalAddress = ExternalRAMInitialAddress + ExternalRAMPhysicalSize;

const size_t SystemRAMBank0InitialAddress = 0xC000;
const size_t SystemRAMBank0PhysicalSize = 0x1000;
constexpr size_t SystemRAMBank0FinalAddress = SystemRAMBank0InitialAddress + SystemRAMBank0PhysicalSize;

const size_t SystemRAMBank1InitialAddress = 0xD000;
const size_t SystemRAMBank1PhysicalSize = 0x1000;
constexpr size_t SystemRAMBank1FinalAddress = SystemRAMBank1InitialAddress + SystemRAMBank1PhysicalSize;

const size_t MirrorRAMInitialAddress = 0xE000;
const size_t MirrorRAMPhysicalSize = 0x1E00;
constexpr size_t MirrorRAMFinalAddress = MirrorRAMInitialAddress + MirrorRAMPhysicalSize;

const size_t IORAMInitialAddress = 0xFF00;
const size_t IORAMPhysicalSize = 0x80;
constexpr size_t IORAMFinalAddress = IORAMInitialAddress + IORAMPhysicalSize;

const size_t HRAMInitialAddress = 0xFF80;
const size_t HRAMPhysicalSize = 0x7F;
constexpr size_t HRAMFinalAddress = HRAMInitialAddress + HRAMPhysicalSize;

}