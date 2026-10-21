@echo off
rem Play Monster Hunter G (Japanese PS2 version) on Windows.
rem   play.bat [quest|easy|village]
rem     (none)   from power-on: logos, title, new game or continue, then the village
rem     quest    straight into G quest 10 (Rathian), normal rules
rem     easy     same, but the hunter cannot faint
rem Your disc files are in the data folder next to mhview_g.exe (copied there by ./mh1 windows DISC); you can also
rem drag your Monster Hunter G .iso onto mhview_g.exe once to install them (about 1.1 GB).
rem Saves: %APPDATA%\mhgpc\memcard0 (MH1_SAVE_DIR overrides; separate from Monster Hunter 1). Logs: %APPDATA%\mhgpc\logs.
rem Keyboard: W/A/S/D move, arrow keys attack, K roll (cross), L sheathe (circle, confirm), J item,
rem E guard, Q camera reset, T/F/G/H d-pad, Enter start / pause. Xbox-style controllers work. Esc quits.
setlocal
cd /d "%~dp0"
rem the window opens because PLAYER.txt is next to mhview_g.exe (without it the G build runs hidden, for tests)
set "MODE="
set "DISC="
:args
if "%~1"=="" goto argsdone
if /i "%~1"=="quest" (set "MODE=quest") else if /i "%~1"=="easy" (set "MODE=easy") else (set "DISC=%~1")
shift
goto args
:argsdone
if not defined DISC if exist disc_dir.txt set /p DISC=<disc_dir.txt
set "DARG="
if defined DISC set DARG="%DISC%"
set "SZ="
if defined MH_SIZE set "SZ=--size %MH_SIZE%"
if /i "%MODE%"=="quest" goto quest
if /i "%MODE%"=="easy" goto easy
mhview_g.exe %DARG% --play %SZ% %MH_ARGS% --boot
goto done
:quest
mhview_g.exe %DARG% --play %SZ% %MH_ARGS% --quest 10
goto done
:easy
set RT_PL_GOD=1
mhview_g.exe %DARG% --play %SZ% %MH_ARGS% --quest 10
goto done
:done
if errorlevel 1 (
  echo.
  echo The game ended with an error. The newest file in %%APPDATA%%\mhgpc\logs holds the debug log;
  echo run bug_report.bat to pack it for a bug report.
  pause
)
