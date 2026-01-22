@echo off
REM Simple build and launch script - same as your old tasks.json

REM Find and run vcvarsall.bat to set up cl.exe
where cl >nul 2>nul
if errorlevel 1 (
    if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
    ) else if exist "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
    ) else if exist "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
    )
)

cl.exe /Zi /EHsc /MD /Fe:bin\baby_gpu.exe /FS /Fobin\ /Fdbin\ /I include src\main.c src\gpu\gpu.c src\platform\platform.c src\driver\driver.c src\driver\shader_assembler.c glfw3.lib opengl32.lib gdi32.lib user32.lib shell32.lib /link /LIBPATH:.

if errorlevel 1 (
    echo Build failed!
    exit /b 1
)

echo Build successful!
echo.
echo Running baby_gpu.exe...
echo ========================
bin\baby_gpu.exe
