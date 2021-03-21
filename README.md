<p align="center">
  <a href="https://github.com/caioso/gbx">
    <img src="https://github.com/caioso/gbx/blob/master/docs/resources/gbx_logo.png" alt="GAME BOY X" >
  </a>
</p>

<h3 align="center">GAME BOY Architectural Exploration Platform</h3>

<p align="center">
  A fresh new approach to GAME BOY Emulation and Game Development
  <br>
  <br>
  <h10 style="font-size:10px">
  8-bit <b>2D Graphics</b> and <b>Sound</b>
  ·
  <b>Polygonal 3D Graphics</b>
  ·
  <b>Expressive Assembly Language</b>
  ·
  <b>IDE</b>
  </h10>
</p>

----------- 

<p align="left">
  <a href="https://github.com/caioso/gbx">
    <img src="https://github.com/caioso/gbx/blob/master/docs/resources/title1.png" alt="What is GAME BOY X?">
  </a>
</p>

**GAME BOY X** is a collection of applications, libraries, tools and conventions intended to explore new possibilities for the GAME BOY platform architecture. GAME BOY X retains full _backwards compatibility_ with the original _DMG_ and _CGB_, while providing new features for game developers, such as _higher fidelity 2D Graphics_, _higher quality audio_, _real-time polygonal 3D graphics_, _improved virtual-hardware_ and a _better development environment_. The GAME BOY X Runtime is intended to be deployable in modern gaming platforms, which allow developers to create authentic retro-style games, while not being too limited by the original platform constraints or development tools/techniques.

<br/>
<p align="left">
  <a href="https://github.com/caioso/gbx">
    <img src="https://github.com/caioso/gbx/blob/master/docs/resources/title2.png" alt="Why GAME BOY X?">
  </a>
</p>

**GAME BOY _Homebrew_** development has gained a considerable momentum in the recent years, but tooling and support remained suboptimal, highly discouraging large-scale GAME BOY game development. Furthermore, a considerable amount of successful indie games have embraced the retro-style aesthetic, but many of them do not fully commit to the limitations of the original platforms they seek to impersonate, giving the final product an inaccurate expression of a retro games.

**GAME BOY X** attempts to address those problems by providing a _modern 8-bit game development platform_, designed after the original GAME BOY architecture. That means developers can creatively develop _authentically-accurate_ retro-style games, with tools and techniques that take inspiration from those used for the development of the original GAME BOY, while not having to fully commit to the _limitations_ imposed by those platforms.

<br/>
<p align="left">
  <a href="https://github.com/caioso/gbx">
    <img src="https://github.com/caioso/gbx/blob/master/docs/resources/title3.png" alt="Feartures">
  </a>
</p>

**GAME BOY X** platform's currently planned features are:

#### Z80X CPU
**Z80X** is a custom Z80-based CPU designed to provide more addressable memory space, faster machine cycles and new instructions, while providing full backwards compatibility with the original Z80 CPU. Games targeting the Z80X platform can take advantage of extra addressing modes, mirror registers, up to 32-bit addressing space, new arithmetic and logic instructions among other features.

#### GBFX, MODE X, HarmoniX, Xpansions
**GAME BOY X** includes a range of new _virtual hardware_ peripherals. The currently planned peripherals are:
- **GBFX**: Advanced Graphics Coprocessor for real time polygonal 3D graphics. Developers can use the coprocessor to render fully textured geometry, providing real 3D graphics to the GAME BOY Platforms (only available for games targeting the GAME BOY X platform)
- **MODE X**: Advanced 2D Graphics rendering mode, providing up to 2MB of character and background memory, while allowing for up to 16 rendering layers (background/foreground) and 128 simultaneously sprites on screen. MODE X also provides native sprite and background/foreground afine transform, per scanline scrolling, 4 sets of 7 color palettes (4 colors each) and dithering transparency. Sprites can also be rendered in a new extended 64x64 pixels mode.
- **HarmoniX**: Extended Audio Synthesis Unit providing 2 Configurable wave channels, with Pulse Wave, Square Wave, Triangule Wave, Sawtooth Wave, 2 Arbitarty wave channels and 2 Noise channels.
- **Xpansions**: Advanced _Code Bank Controller_ allowing developers to create DLC-like expansions for games targeting the GAME BOY X platform.

#### GBXpressive Assembly Language
The GAME BOY X requires developers to use Z80's assembly language for game development. In order to make the development friendlier and more approachable to newcomers, a new _expressive_ assembly language has been developed. Using the GBXASM assembler, developers can create debug and test games targeting the DMG, CGB and GBX platforms.

#### GBX Builder
Development IDE providing a syntax-highlighted Code Editor, Graphical Debugger, Graphics Editor (2D/3D) and Sound Editor.

<br/>
<p align="left">
  <a href="https://github.com/caioso/gbx">
    <img src="https://github.com/caioso/gbx/blob/master/docs/resources/title4.png" alt="Project Status">
  </a>
</p>

This repository is currently in active development. The code is constantly changed and new features will be added as the development proceeds. ***the project has currently no release date planned and no releases available***. As soon as they become available , you'll be able to download them from this page.

---------
</p>
<p align="center">
  <a href="https://github.com/caioso/gbx">
    <img src="https://github.com/caioso/gbx/blob/master/docs/resources/footer.png" alt="Bootstrap logo" width="100%">
  </a>
</p>
Graphics by <a href="https://www.behance.net/marcelso">[The UI Soup]</a>
