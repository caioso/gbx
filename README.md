# GAME BOY X Platform
GAME BOY Architectural Exploration Platform


### Project Status
This repository is currently in active development. The code is constantly changed and new features will be added as the development proceeds. ***the project has currently no release date planned and no releases available***. As soon as they become available , you'll be able to download them from this page.

### Project Description
GAME BOY X is a retro-style game engine, backwards compatible with Nintendo's GAME BOY (DMG) and GAME BOY Color (CGB) platforms. The engine is designed around the archietcture principles of the GAME BOY (limited memory resources, graphics, sound, input and programming language) which allows for genuine 8-bit style game development for modern platform. The engine is planned to be ported to moder game platforms (such as the Sony Playstation 5, Nintendo Switch and Microsoft XBOX Series X/S).

Internally, GAME BOY X implements a Zilog Z80 processor core, along with a main graphics processing unit, audio processing unit, memory controller and peripherals as described in the Nintendo GAME BOY platform. Alonmg with that, the engine provides an advanced execution mode (Mode X) where larger screen resolution, more RAM memory and ROM space and extra peripherals are made available to the developer, to allow for the development of games never before possible to be played in the original DMG/CGB. The current planned featrures are:

- Extended Z80 Instruction Set (codenamed Z80X); This includes:
  - Extra *mirror-register bank*
  - Support for conditional instruction execution
  - Multiplication/Division Instructions
  - Indexed addressing mode
  - Advanced Memory Controller (32-bit ROM & RAM memory mapping)
    - Extended Addressing mode coupled with a *long-range fast addressing mode*
    - Advanced Code Bank Controller
- AGU (Advanced Graphipcs Unit; codenamed GBFX), supporting:
  - Extended screen resolution  (256 x 144 pixes, 16:9 screen aspect ratio)
  - Native Support to texture-mapped 3D geometry
    - Naitive 3D renderer
    - Native lighting engine
      - Shadow-rendeing with dithering
    - Support for texture-mapped polygons
    - Support for polygon rendering on background/foreground layer 
    - Support for polygon rendering on sprite-object
  - Up to 16 background/foreground layes
    - Native support for affine transformation
    - Native support for layer transparency
    - Individual scanline transformation
  - Up to 4 sets of 16 color each (2 sets for sprites, 2 sets for backgrounds)
  - Support for up to 128 on-screen sprites
    - Extra 64x64 pixels sprite rendering mode
- ASU (Advanced Synth Unit; codenamed GBHarmoniX), supporting:
  - 2 Configurable wave channels, including
    - Pulse Wave
    - Square Wave
    - Trangule Wave
    - Sawtooth Wave
  - 2 Arbitarty wave channels
  - 2 Noise channels
- Advanced debugging capabilities with *native* debug instructions
- Integrated Development Environment (codenamed GBX Builder)
  - Rich Text editor and project management
  - Assembler
  - 2D/3D graphics editor
  - Audio Editor
  - Deployment Flow and Tools
- Expressive Assembly (codename GBXpressive)
  - Own assembly language, providing on expressive assembly-like statements
  - Compatible with DMG and CGB systems
- Full backwards compatibility with Nintendo GAME BOY and GAM BOY Color systems

