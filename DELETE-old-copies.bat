@echo off
REM ---------------------------------------------------------------------------
REM  Removes the four leftover source files in this folder that nothing builds.
REM
REM  They are November drafts from before the project lived in one folder. None
REM  of them is listed in GameBuilding.vcxproj. One of them, iMain_2.cpp, has
REM  already broken the build once: it got added to the project and its
REM  #include "Enemy.h" picked up an old 6.9 KB copy instead of the real one.
REM
REM  This deletes exactly these four files and nothing else. Read it, then
REM  double click it. Nothing else in the folder is touched.
REM ---------------------------------------------------------------------------
echo.
echo About to delete, from this folder only:
echo    Audio_1.h
echo    Enemy_3.h
echo    iMain_2.cpp
echo    iMain_3.cpp
echo.
choice /M "Delete these four files"
if errorlevel 2 goto :cancelled

if exist "Audio_1.h"   del /f /q "Audio_1.h"
if exist "Enemy_3.h"   del /f /q "Enemy_3.h"
if exist "iMain_2.cpp" del /f /q "iMain_2.cpp"
if exist "iMain_3.cpp" del /f /q "iMain_3.cpp"

echo.
echo Done. If these were already committed to git, also run:
echo    git rm --cached Audio_1.h Enemy_3.h iMain_2.cpp iMain_3.cpp
goto :done

:cancelled
echo Cancelled. Nothing was deleted.

:done
echo.
pause
