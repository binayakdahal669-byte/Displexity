@echo off
powershell -Command "Start-Process neovide -ArgumentList '-- -u %~dp0nvim\init.lua' -Verb RunAs"
