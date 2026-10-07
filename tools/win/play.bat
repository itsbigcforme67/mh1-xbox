@echo off
rem Play the Windows build (same options as tools/play.sh).
rem   play.bat [quest|easy|village] [DISC_DIR]
rem     (none)   from power-on: logos, title, new game or continue, then the village
rem     quest    straight into quest 10 (Rathian), normal rules
rem     easy     same, but the hunter cannot faint
rem     village  straight into Kokoto village (no save data)
rem DISC_DIR holds your own AFS_DATA.AFS and SLPM_654.95 (and AFS00.AFS / AFS01.AFS for sound),
rem extracted from your own copy of the Japanese game (docs/pc.md). Without the argument it is
rem the first line of disc_dir.txt next to this file, else the folder "disc" next to it.
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
if not defined DISC if exist disc\AFS_DATA.AFS set "DISC=disc"
if not defined DISC goto nodisc
if not exist "%DISC%\AFS_DATA.AFS" goto nodisc
if not defined MH_SIZE set "MH_SIZE=1024x768"
if /i "%MODE%"=="quest" goto quest
if /i "%MODE%"=="easy" goto easy
if /i "%MODE%"=="village" goto village
mhview.exe "%DISC%" --play --size %MH_SIZE% --boot
goto done
:quest
mhview.exe "%DISC%" --play --size %MH_SIZE% --quest 10
goto done
:easy
set RT_PL_GOD=1
mhview.exe "%DISC%" --play --size %MH_SIZE% --quest 10
goto done
:village
set RT_VILLAGE_START=1
set RT_VILLAGE_SKIP_INTRO=1
mhview.exe "%DISC%" --play --size %MH_SIZE% --quest 10
goto done
:nodisc
echo Cannot find your game files.
echo Put the extracted disc files (AFS_DATA.AFS, SLPM_654.95, AFS00.AFS, AFS01.AFS) in a folder and either
echo   - run:  play.bat C:\path\to\that\folder
echo   - or write that folder's path as the first line of disc_dir.txt next to play.bat
echo   - or name the folder "disc" and keep it next to play.bat.
pause
exit /b 1
:done
if errorlevel 1 (
  echo.
  echo The game ended with an error. The newest file in %%APPDATA%%\mh1pc\logs holds the debug log;
  echo run bug_report.bat to pack it for a bug report.
  pause
)
