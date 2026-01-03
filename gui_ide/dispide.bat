@echo off
setlocal

:: Set NVIM config path to our custom config
set SCRIPT_DIR=%~dp0
set XDG_CONFIG_HOME=%SCRIPT_DIR%
set XDG_DATA_HOME=%SCRIPT_DIR%data
set XDG_STATE_HOME=%SCRIPT_DIR%state

:: Try Neovide first (has wiggly cursor effects)
where neovide >nul 2>&1
if %errorlevel% equ 0 (
    if "%~1"=="" (
        start "" neovide --neovim-bin nvim
    ) else (
        start "" neovide --neovim-bin nvim "%~1"
    )
    exit /b 0
)

:: Fall back to nvim in terminal
where nvim >nul 2>&1
if %errorlevel% neq 0 (
    echo Neovim not found. Please install Neovim or Neovide.
    echo Neovim: https://neovim.io/
    echo Neovide: https://neovide.dev/ ^(has wiggly cursor!^)
    pause
    exit /b 1
)

if "%~1"=="" (
    nvim
) else (
    nvim "%~1"
)
