@echo off
REM Update source for vulkan-headers

REM
REM Copyright 2016 The Android Open Source Project
REM Copyright (C) 2015 Valve Corporation
REM
REM Licensed under the Apache License, Version 2.0 (the "License");
REM you may not use this file except in compliance with the License.
REM You may obtain a copy of the License at
REM
REM      http://www.apache.org/licenses/LICENSE-2.0
REM
REM Unless required by applicable law or agreed to in writing, software
REM distributed under the License is distributed on an "AS IS" BASIS,
REM WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
REM See the License for the specific language governing permissions and
REM limitations under the License.
REM

setlocal EnableDelayedExpansion
set errorCode=0
set ANDROID_BUILD_DIR=%~dp0
set BUILD_DIR=%ANDROID_BUILD_DIR%
set BASE_DIR=%BUILD_DIR%\third_party
set VULKAN_HEADERS_DIR=%BASE_DIR%\Vulkan-Headers

for %%X in (where.exe) do (set FOUND=%%~$PATH:X)
if not defined FOUND (
   echo Dependency check failed:
   echo   where.exe not found
   echo   This script requires Windows Vista or later, which includes where.exe.
   set errorCode=1
)

where /q git.exe
if %ERRORLEVEL% equ 1 (
   echo Dependency check failed:
   echo   git.exe not found
   echo   Git for Windows can be downloaded here:  https://git-scm.com/download/win
   echo   Install and ensure git.exe makes it into your PATH
   set errorCode=1
)

where /q ndk-build.cmd
if %ERRORLEVEL% equ 1 (
   echo Dependency check failed:
   echo   ndk-build.cmd not found
   echo   Android NDK can be downloaded here:  http://developer.android.com/ndk/guides/setup.html
   echo   Install and ensure ndk-build.cmd makes it into your PATH
   set errorCode=1
)

REM ensure where is working with below false test
REM where /q foo
REM if %ERRORLEVEL% equ 1 (
REM echo foo
REM )

:main

if %errorCode% neq 0 (goto:error)

REM Read the target versions from external file, which is shared with Linux script

if not exist %ANDROID_BUILD_DIR%\vulkan-headers_revision_android (
   echo.
   echo Missing vulkan-headers_revision_android file. Place it in %ANDROID_BUILD_DIR%
   set errorCode=1
   goto:error
)

set /p VULKAN_HEADERS_REVISION= < vulkan-headers_revision_android
echo VULKAN_HEADERS_REVISION=%VULKAN_HEADERS_REVISION%


echo Creating vulkan-headers in %BASE_DIR%

set sync-vulkan-headers=1

if %sync-vulkan-headers% equ 1 (
   if %ERRORLEVEL% neq 0 (goto:error)
   if not exist %VULKAN_HEADERS_DIR% (
      call:create_vulkan-headers
   )
   if %errorCode% neq 0 (goto:error)
   call:update_vulkan-headers
   if %errorCode% neq 0 (goto:error)
)

echo.
echo Exiting
goto:finish

:error
echo.
echo Halting due to error
goto:finish

:finish
if not "%cd%\" == "%BUILD_DIR%" ( cd %BUILD_DIR% )
endlocal
REM This needs a fix to return error, something like exit %errorCode%
REM Right now it is returning 0
goto:eof



REM // ======== Functions ======== //

:create_vulkan-headers
   echo.
   echo Creating local vulkan-headers repository %VULKAN_HEADERS_DIR%
   if not exist "%VULKAN_HEADERS_DIR%\" mkdir %VULKAN_HEADERS_DIR%
   cd %VULKAN_HEADERS_DIR%
   git clone https://github.com/KhronosGroup/Vulkan-Headers.git .
   git checkout %VULKAN_HEADERS_REVISION%
   if not exist %VULKAN_HEADERS_DIR%\registry (
      echo vulkan-headers source download failed!
      set errorCode=1
   )
goto:eof

:update_vulkan-headers
   echo.
   echo Updating %VULKAN_HEADERS_DIR%
   cd %VULKAN_HEADERS_DIR%
   git fetch --all
   git checkout %VULKAN_HEADERS_REVISION%
   if not exist %VULKAN_HEADERS_DIR%\registry (
      echo vulkan-headers source update failed!
      set errorCode=1
   )
goto:eof
