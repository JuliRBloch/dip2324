@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -arch=x64
cl.exe /EHsc /Zi /Fe:MyProgram.exe main.cpp Dip2.cpp /I "C:/Users/Juli/OneDrive/Escritorio/EDA/opencv/build/include" /link /LIBPATH:"C:/Users/Juli/OneDrive/Escritorio/EDA/opencv/build/x64/vc16/lib" opencv_world480d.lib /machine:x64
