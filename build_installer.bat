@echo off
setlocal

REM ============================================
REM  Chrono - Build Installer Script
REM  Uses Inno Setup to compile the installer
REM ============================================

set "PROJECT_ROOT=%~dp0"
set "INSTALLER_DIR=%PROJECT_ROOT%Installer Submission"
set "GAME_DIR=%INSTALLER_DIR%\GAMEDIRECTORY"
set "BUILD_DIR=%PROJECT_ROOT%ChronoGame\build"

REM --- Locate Inno Setup (ISCC.exe) ---
set "ISCC="
if exist "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" (
    set "ISCC=C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
) else if exist "C:\Program Files (x86)\Inno Setup 5\ISCC.exe" (
    set "ISCC=C:\Program Files (x86)\Inno Setup 5\ISCC.exe"
) else if exist "C:\Program Files\Inno Setup 6\ISCC.exe" (
    set "ISCC=C:\Program Files\Inno Setup 6\ISCC.exe"
) else if exist "C:\Program Files\Inno Setup 5\ISCC.exe" (
    set "ISCC=C:\Program Files\Inno Setup 5\ISCC.exe"
)

if "%ISCC%"=="" (
    echo ERROR: Inno Setup (ISCC.exe) not found.
    echo Please install Inno Setup from https://jrsoftware.org/isdl.php
    exit /b 1
)

echo [1/3] Found Inno Setup: %ISCC%

REM --- Step 1: Build the project with MSBuild ---
echo [2/3] Building ChronoGame in Release configuration...

REM Locate MSBuild
set "MSBUILD="
for /f "usebackq delims=" %%i in (`where msbuild 2^>nul`) do set "MSBUILD=%%i"& goto :found_msbuild
:found_msbuild

if "%MSBUILD%"=="" (
    REM Try Visual Studio 2022 default path
    for /f "usebackq delims=" %%i in (`dir /b /ad "C:\Program Files\Microsoft Visual Studio\2022\*" 2^>nul`) do (
        if exist "C:\Program Files\Microsoft Visual Studio\2022\%%i\MSBuild\Current\Bin\MSBuild.exe" (
            set "MSBUILD=C:\Program Files\Microsoft Visual Studio\2022\%%i\MSBuild\Current\Bin\MSBuild.exe"
            goto :msbuild_found
        )
    )
)
:msbuild_found

if "%MSBUILD%"=="" (
    echo ERROR: MSBuild not found. Please run this from a Developer Command Prompt.
    exit /b 1
)

echo       Using MSBuild: %MSBUILD%
"%MSBUILD%" "%PROJECT_ROOT%ChronoGame\ChronoGame.sln" /p:Configuration=Release /p:Platform=x64 /m /v:minimal
if %errorlevel% neq 0 (
    echo ERROR: Build failed with error code %errorlevel%.
    exit /b %errorlevel%
)

echo       Build succeeded.

REM --- Step 2: Prepare GAMEDIRECTORY ---
echo [3/3] Preparing GAMEDIRECTORY...

REM Clean previous GAMEDIRECTORY if it exists
if exist "%GAME_DIR%" (
    echo       Cleaning previous GAMEDIRECTORY...
    rmdir /s /q "%GAME_DIR%"
)
mkdir "%GAME_DIR%"

REM Copy built executables and DLLs from the build output
REM Adjust these paths based on your actual build output location
set "BUILD_OUTPUT=%PROJECT_ROOT%ChronoGame\Release"
if not exist "%BUILD_OUTPUT%" (
    set "BUILD_OUTPUT=%PROJECT_ROOT%ChronoGame\x64\Release"
)
if not exist "%BUILD_OUTPUT%" (
    set "BUILD_OUTPUT=%PROJECT_ROOT%ChronoGame\build"
)

echo       Copying from: %BUILD_OUTPUT%

REM Copy executables
if exist "%BUILD_OUTPUT%\*.exe" (
    copy /y "%BUILD_OUTPUT%\*.exe" "%GAME_DIR%\"
) else (
    echo WARNING: No .exe files found in %BUILD_OUTPUT%
)

REM Copy DLLs
if exist "%BUILD_OUTPUT%\*.dll" (
    copy /y "%BUILD_OUTPUT%\*.dll" "%GAME_DIR%\"
)

REM Copy game assets
if exist "%PROJECT_ROOT%Assets" (
    echo       Copying Assets...
    xcopy /e /i /y /q "%PROJECT_ROOT%Assets" "%GAME_DIR%\Assets"
)

REM Copy the game icon for the installed shortcut
if exist "%INSTALLER_DIR%\INSTALLERFILES\Chrono.ico" (
    copy /y "%INSTALLER_DIR%\INSTALLERFILES\Chrono.ico" "%GAME_DIR%\"
)

echo       GAMEDIRECTORY prepared.

REM --- Step 3: Compile the installer ---
echo [4/3] Compiling installer with Inno Setup...

pushd "%INSTALLER_DIR%"
"%ISCC%" "InstallScript.iss"
set "ISCC_RESULT=%errorlevel%"
popd

if %ISCC_RESULT% neq 0 (
    echo ERROR: Inno Setup compilation failed with error code %ISCC_RESULT%.
    exit /b %ISCC_RESULT%
)

echo.
echo ============================================
echo  Installer built successfully!
echo  Output: %INSTALLER_DIR%\INSTALLER\Chrono_Setup.exe
echo ============================================

endlocal
exit /b 0
