@echo off
title Plasma Game Engine Builder
mode con: cols=98 lines=33
echo.
echo  /$$$$$$$  /$$        /$$$$$$   /$$$$$$  /$$      /$$  /$$$$$$ 
echo ^| $$__  $$^| $$       /$$__  $$ /$$__  $$^| $$$    /$$$ /$$__  $$
echo ^| $$  \ $$^| $$      ^| $$  \ $$^| $$  \__/^| $$$$  /$$$$^| $$  \ $$
echo ^| $$$$$$$/^| $$      ^| $$$$$$$$^|  $$$$$$ ^| $$ $$/$$ $$^| $$$$$$$$
echo ^| $$____/ ^| $$      ^| $$__  $$ \____  $$^| $$  $$$^| $$^| $$__  $$
echo ^| $$      ^| $$      ^| $$  ^| $$ /$$  \ $$^| $$\  $ ^| $$^| $$  ^| $$
echo ^| $$      ^| $$$$$$$$^| $$  ^| $$^|  $$$$$$/^| $$ \/  ^| $$^| $$  ^| $$
echo ^|__/      ^|________/^|__/  ^|__/ \______/ ^|__/     ^|__/^|__/  ^|__/
echo.
echo.
echo Version: 1.0.0               Made By: Kutline
echo Last Modified: 08/06/2020
echo Last Change: 
timeout 2 >nul
echo Running initial npm package check
node --version >nul 2>&1 && ( echo Node.JS Is Installed) || ( goto nodejs )
node --version
IF exist node_modules/nul ( echo node_modules exists ) ELSE ( echo npm packages not installed. Running installer... && npm install)

:choice
cls
echo ====================================================================
echo                             BUILD OPTIONS
echo ====================================================================
echo.
echo  Press 1-3 on your keyboard to build the game engine.
echo  Press 'Q' to exit the script.
echo.
echo  [1] Generate Visual Studio 2019 Build (Standard)
echo  [2] Build All (Takes Longer)
echo  [3] Reinstall npm Packages
echo.
echo  [Q] Exit Script

choice /c 123q /t 30 /d q /n
IF %ERRORLEVEL% EQU 1 goto OPT1
IF %ERRORLEVEL% EQU 2 goto OPT2
IF %ERRORLEVEL% EQU 3 goto OPT3
IF %ERRORLEVEL% EQU 4 goto end
goto end

:OPT1
cls
echo ====================================================================
echo                             BUILD OPTIONS
echo ====================================================================
echo.
echo Generating Visual Studio 2019 Build...
node index.js cmake --builder="Visual Studio 16 2019"
pause
goto choice

:OPT2
cls
echo ====================================================================
echo                             BUILD OPTIONS
echo ====================================================================
echo.
echo Running all scripts...
npm start
pause
goto choice

:OPT3
cls
echo ====================================================================
echo                             BUILD OPTIONS
echo ====================================================================
echo.
echo Reinstalling npm packages. 
echo Once completed, press any key to check installed packages.
start npm install
pause
start npm list --depth=0
goto choice

:nodejs
cls
echo ====================================================================
echo                         PREREQUISITES NOT MET
echo ====================================================================
echo.
echo Node.js is not installed and it required to install npm packages.
echo.
echo  [1] Go to download site
echo  [2] Exit Script
choice /c 12 /t 30 /d 2 /n
IF ERRORLEVEL 1 start https://nodejs.org/en/download/
IF ERRORLEVEL 2 goto end
exit

:end
echo Exiting...
exit
