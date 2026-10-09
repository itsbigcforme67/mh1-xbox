@echo off
rem Play the Windows build (same options as tools/play.sh).
rem   play.bat [quest|easy|village] [DISC_DIR]
rem     (none)   from power-on: logos, title, new game or continue, then the village
rem     quest    straight into quest 10 (Rathian), normal rules
rem     easy     same, but the hunter cannot faint
rem     village  straight into Kokoto village (no save data)
rem DISC_DIR is your own Japanese Monster Hunter disc image (.iso: installed once, about 925 MB, into the
rem data folder next to mhview.exe or %APPDATA%\mh1pc\data) or an already extracted folder. Without it:
rem the first line of disc_dir.txt, else the installed data, else you are asked for the ISO.
rem Saves: %APPDATA%\mh1pc\memcard0 (MH1_SAVE_DIR overrides). Debug logs: %APPDATA%\mh1pc\logs.
rem Keyboard: W/A/S/D move, arrow keys attack, K roll (cross), L sheathe (circle, confirm), J item,
rem E guard, Q camera reset, T/F/G/H d-pad, Enter start / pause. Xbox-style controllers work. Esc quits.
setlocal
cd /d "%~dp0"
set "MODE="
set "DISC="
:args
if "%~1"=="" goto argsdone
if /i "%~1"=="quest" (set "MODE=quest") else if /i "%~1"=="easy" (set "MODE=easy") else if /i "%~1"=="village" (set "MODE=village") else (set "DISC=%~1")
shift
goto args
:argsdone
if not defined DISC if exist disc_dir.txt set /p DISC=<disc_dir.txt
rem no data folder given: mhview.exe finds its data folder itself, or asks for the ISO (drag it onto the window)
set "DARG="
if defined DISC set DARG="%DISC%"
set "SZ="
if defined MH_SIZE set "SZ=--size %MH_SIZE%"
if /i "%MODE%"=="quest" goto quest
if /i "%MODE%"=="easy" goto easy
if /i "%MODE%"=="village" goto village
mhview.exe %DARG% --play %SZ% %MH_ARGS% --boot
goto done
:quest
mhview.exe %DARG% --play %SZ% %MH_ARGS% --quest 10
goto done
:easy
set RT_PL_GOD=1
mhview.exe %DARG% --play %SZ% %MH_ARGS% --quest 10
goto done
:village
set RT_VILLAGE_START=1
set RT_VILLAGE_SKIP_INTRO=1
mhview.exe %DARG% --play %SZ% %MH_ARGS% --quest 10
goto done
:done
if errorlevel 1 (
  echo.
  echo The game ended with an error. The newest file in %%APPDATA%%\mh1pc\logs holds the debug log;
  echo run bug_report.bat to pack it for a bug report.
  pause
)
