# Languages

[English](README.md) [简体中文](README.SC.md) [繁體中文](README.TC.md)

Notes: I use Simplified Chinese in daily life, so the translation isn't 100% accurate.

# Description

This project is similar to tkinter. It's an interface to Tcl/Tk for C++ 11 and higher, with some Tk controls inside.

# Notes

Thanks to Python and Tcl/Tk!

Currently MSVC only, exploring porting to MinGW and other platforms.

# Build

1. Download Tcl/Tk 8.6.15 (Because I'm too lazy to ) from [here](www.tcl.tk) or [here](www.tcl-lang.org). Remember to download both Tcl and Tk!

2. Extract them.

3. Modify Line 21 ~ 23 in the file `tcl_compile.bat`.

4. If you want to change configurations of Tcl/Tk, I suggest that you should contact with me. Because it's hard to say it clearly.

5. Run the `.bat` file. If you don't trust me, then check the content.

6. Launch Visual Studio. I preferred Visual Studio 2022. Then open the project (`tk4cpp.sln`). Start your build!