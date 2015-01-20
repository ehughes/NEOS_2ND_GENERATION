@echo off

del run.bat
mkdir output

setlocal disableDelayedExpansion
for /f "delims=" %%A in ('forfiles /m *.wav /c "cmd /c echo @relpath"') do (
  set "file=%%~A"
  setlocal enableDelayedExpansion
  echo sox !file:~2! -b8 output\!file:~2! >> run.bat
  endlocal
)

echo pause >> run.bat

pause
