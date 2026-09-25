@echo off
title Contact Management System Pro - GUI
cd /d "%~dp0"
echo ========================================================
echo Starting Contact Management System Pro GUI...
echo Opening http://localhost:5050 in your browser...
echo ========================================================
start "" http://localhost:5050
python server.py
pause
