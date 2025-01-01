@echo off

del /Q /F /A *.suo >nul
del /Q *.user >nul
del /Q *.ncb >nul

del /Q /F /S Debug >nul
del /Q /F /S Release >nul


exit