# CPU Organization
 
## CPU Mode
Execution mode refers to the internal CPU organization when running applications. There are two execution modes: Emulation and Execution. Emulation reproduces DMG and CGB CPU organization, providing a reduced registers set, a reduced set of peripheral access, a reduced instruction set, and restricted memory mapping. This mode is meant to execute code written for the DMG/CGB platforms. Execution mode is used to run _native-applications_ under the GBX environment. System calls can only be performed when running in Execution Mode.
Emulation Modes provides a 16-bit _banked_ addressing space, while Execution mode provides a 32-bit _banked_ addressing space. Execution mode includes a second set of all-purpose registers, indexed addressing mode, cross-domain memory access and access to GBFX, HarmoniX, Expansion Controller peripherals. At bootup, the BIOS determines the target CPU Mode, based on the ROM image format. Once booted in a specific CPU Mode, it is not possible to change it. At power-up, the CPU starts in Execution mode.
 
## CPU Domain
Domains are intended to separate the execution environment into two slices. There are two CPU domains: System and User. The System domain includes resources that should normally be outside of the application control, for security reasons. Those resources include special registers, special instructions, the system BIOS and the GBOS. Only the system BIOS and GBOS execute under the System Domain. The User application can interact with resources allocated to the System domain by issuing system-calls towards the operating system. In the User domain, applications execute in a _sandboxed_ environment.
 
### Native Application
Application designed to run under GBX Execution mode. Those application can make use of extra registers, peripherals and instructions not available in Emulation mode.
 
### Emulated Applications
Applications designed to run in DMG or CGB platforms.