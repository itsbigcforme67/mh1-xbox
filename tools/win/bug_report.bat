@echo off
rem Make a bug report: zips the newest two debug logs and the save's metadata (file names, sizes, dates;
rem NOT the save itself unless you run: bug_report.bat --with-save) into mh1_bug_report_<date>.zip on your
rem desktop-independent current folder, then prints where to send it. Needs Windows PowerShell (Windows 10+).
powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0bug_report.ps1" %*
pause
