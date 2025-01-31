# 語言

[English](README.md) [简体中文](README.SC.md) [繁體中文](README.TC.md)

注：本人日常使用簡體中文，因此翻譯不 100% 準確。

# 介紹

這個專案類似於 Tkinter，是一個為 C++ 11 及以上寫的 Tcl/Tk 接口，包含一些 Tk 内建的控制項。

# 附注

感謝 Python 和 Tkinter！

目前僅支持 MSVC，正在探索 MinGW 和其他平臺的移植。

# 構建

1. 從[這裡](www.tcl.tk)或[這裡](www.tcl-lang.org)下載 Tcl/Tk 8.6.15（因為我懶得兼容更多版本了）。記得 Tcl 和 Tk 兩個都要下載！

2. 把它們解壓。

3. 修改檔案 `tcl_compile.bat` 的第 21 到 23 行。

4. 運行這個 `.bat` 檔案。如果你不信任我, 可以檢查檔案内容。

5. 啟動 Visual Studio（建议使用 Visual Studio 2022）。然後開啟專案（`tk4cpp.sln`）。開始你的構建！