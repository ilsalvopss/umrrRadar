# umrrRadar
Parser and logger for a SmartMicro *UMRR-0A0703-220701-050704* in RS485 mode.

## features
 - light, native UI
 - parser for smartMicro proprietary protocol
 - checksum validation
 - auto hardware detection
 - auto COM detection
 - multiple averaging algorithms
 - modern c++ codebase

## OS compatibility
 Windows 7 up to 11. Auto-detection requires the serial connection to be delivered though USB (virtual serial port).

## building from source
 This software is built with the MSYS64 environment in mind. Refer to CMakeLists.txt to details.

 To build from source you need to have previously compiled as a **static library** (and installed):
  - wxWidgets
  - wxMathPlot
  - the manifest.o, which is included prebuild for ease of use (can be omitted if you don't care about high-DPI resolution)

Then, in the root directory run:

    cmake . -G "MSYS Generators"
    make