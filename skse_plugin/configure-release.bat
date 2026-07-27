@echo off
setlocal
set "VCPKG_ROOT=C:\Nolvus\_vcpkg"
set "VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
set "CMAKE=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
set "NINJA_DIR=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
set "CL_EXE=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC\14.41.34120\bin\Hostx64\x64\cl.exe"
set "OUTDIR=C:/Nolvus/Instances/Nolvus Awakening/MODS/mods/Dev - Spell Hotbar 2"
set "PLUGIN=C:\Nolvus\Projects\spell-hotbar-2\skse_plugin"

call "%VCVARS%" || exit /b 1
set "PATH=%NINJA_DIR%;%PATH%"
cd /d "%PLUGIN%" || exit /b 1
if exist "build\release" rmdir /s /q "build\release"
"%CMAKE%" --preset release "-DOUTPUT_FOLDER=%OUTDIR%" "-DCMAKE_CXX_COMPILER=%CL_EXE%"
exit /b %ERRORLEVEL%
