@echo off
setlocal enabledelayedexpansion

REM ============================================
REM  Chrono - Build Installer Script
REM  Uses Inno Setup to compile the installer
REM ============================================

set "PROJECT_ROOT=%~dp0"
set "INSTALLER_DIR=%PROJECT_ROOT%Installer Submission"
set "GAME_DIR=%INSTALLER_DIR%\GAMEDIRECTORY"

REM --- Locate Inno Setup (ISCC.exe) ---
set "ISCC="
if exist "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" (
    set "ISCC=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
)
if not defined ISCC (
    if exist "C:\Program Files (x86)\Inno Setup 5\ISCC.exe" (
        set "ISCC=C:\Program Files (x86)\Inno Setup 5\ISCC.exe"
    )
)
if not defined ISCC (
    if exist "C:\Program Files\Inno Setup 6\ISCC.exe" (
        set "ISCC=C:\Program Files\Inno Setup 6\ISCC.exe"
    )
)
if not defined ISCC (
    if exist "C:\Program Files\Inno Setup 5\ISCC.exe" (
        set "ISCC=C:\Program Files\Inno Setup 5\ISCC.exe"
    )
)
if not defined ISCC (
    echo ERROR: Inno Setup ^(ISCC.exe^) not found.
    echo Please install Inno Setup from https://jrsoftware.org/isdl.php
    exit /b 1
)

echo [1/4] Found Inno Setup: !ISCC!

REM --- Step 1: Build the project with MSBuild ---
echo [2/4] Building ChronoGame in Release configuration...

set "MSBUILD="
where msbuild >nul 2>&1
if !errorlevel! equ 0 (
    for /f "usebackq delims=" %%i in (`where msbuild`) do (
        set "MSBUILD=%%i"
    )
)
if not defined MSBUILD (
    for /d %%v in ("C:\Program Files\Microsoft Visual Studio\2022\*") do (
        if exist "%%v\MSBuild\Current\Bin\MSBuild.exe" (
            set "MSBUILD=%%v\MSBuild\Current\Bin\MSBuild.exe"
        )
    )
)
if not defined MSBUILD (
    echo ERROR: MSBuild not found. Please run from a Developer Command Prompt.
    exit /b 1
)

echo       Using MSBuild: !MSBUILD!
"!MSBUILD!" "%PROJECT_ROOT%ChronoGame\ChronoGame.sln" /p:Configuration=Release /p:Platform=x64 /m /v:minimal
if !errorlevel! neq 0 (
    echo ERROR: Build failed with error code !errorlevel!.
    exit /b !errorlevel!
)
echo       Build succeeded.

REM --- Step 2: Prepare GAMEDIRECTORY ---
echo [3/4] Preparing GAMEDIRECTORY...

if exist "!GAME_DIR!" (
    echo       Cleaning previous GAMEDIRECTORY...
    rmdir /s /q "!GAME_DIR!"
)
mkdir "!GAME_DIR!"

REM Locate build output
set "BUILD_OUTPUT="
if exist "%PROJECT_ROOT%ChronoGame\Release" (
    set "BUILD_OUTPUT=%PROJECT_ROOT%ChronoGame\Release"
)
if not defined BUILD_OUTPUT (
    if exist "%PROJECT_ROOT%ChronoGame\x64\Release" (
        set "BUILD_OUTPUT=%PROJECT_ROOT%ChronoGame\x64\Release"
    )
)
if not defined BUILD_OUTPUT (
    if exist "%PROJECT_ROOT%ChronoGame\build" (
        set "BUILD_OUTPUT=%PROJECT_ROOT%ChronoGame\build"
    )
)

echo       Copying from: !BUILD_OUTPUT!

REM Copy executables and DLLs
if defined BUILD_OUTPUT (
    copy /y "!BUILD_OUTPUT!\*.exe" "!GAME_DIR!\" >nul 2>&1
    copy /y "!BUILD_OUTPUT!\*.dll" "!GAME_DIR!\" >nul 2>&1
) else (
    echo WARNING: No build output directory found.
)

REM Copy game assets
if exist "%PROJECT_ROOT%Assets" (
    echo       Copying Assets...
    xcopy /e /i /y /q "%PROJECT_ROOT%Assets" "!GAME_DIR!\Assets" >nul
)

REM Copy the game icon
if exist "%INSTALLER_DIR%\INSTALLERFILES\Chrono.ico" (
    copy /y "%INSTALLER_DIR%\INSTALLERFILES\Chrono.ico" "!GAME_DIR!\" >nul
)
echo       GAMEDIRECTORY prepared.

REM --- Step 3: Compile the installer ---
echo [4/4] Compiling installer with Inno Setup...

pushd "!INSTALLER_DIR!"
"!ISCC!" "InstallScript.iss"
set "ISCC_RESULT=!errorlevel!"
popd

if !ISCC_RESULT! neq 0 (
    echo ERROR: Inno Setup compilation failed with error code !ISCC_RESULT!.
    exit /b !ISCC_RESULT!
)

echo.
echo ============================================
echo  Installer built successfully!
echo  Output: !INSTALLER_DIR!\INSTALLER\Chrono_Setup.exe
echo ============================================

endlocal
exit /b 0
