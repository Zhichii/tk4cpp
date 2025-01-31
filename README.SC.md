# 语言

[English](README.md) [简体中文](README.SC.md) [繁體中文](README.TC.md)

注：本人日常使用简体中文，因此翻译不 100% 准确。

# 介绍

这个项目类似于 Tkinter，是一个为 C++ 11 及以上写的 Tcl/Tk 接口，包含一些 Tk 内置的控件。

# 附注

感谢 Python 和 Tkinter！

目前仅支持 MSVC，正在探索 MinGW 和其它平台的移植。

# 构建

1. 从[这里](www.tcl.tk)或[这里](www.tcl-lang.org)下载 Tcl/Tk 8.6.15（因为我懒得兼容更多版本了）。记得 Tcl 和 Tk 两个都要下载！

2. 把它们解压。

3. 修改文件 `tcl_compile.bat` 的第 21 到 23 行。

4. 运行这个 `.bat` 文件。如果你不信任我, 可以检查文件内容。

5. 启动 Visual Studio（建议使用 Visual Studio 2022）。然后打开项目（`tk4cpp.sln`）。开始你的构建！