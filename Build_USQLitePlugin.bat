@echo off

rem ## USQLite Plugin Complier
rem ## Check Unreal4 InstallFolder
rem ## 

set UE4_Install=F:\Program Files\Epic Games\UE_4.22
set UE4_UAT=Engine\Build\BatchFiles\RunUAT.bat
set TARGET_FOLDER=%~dp0\Plugins\USQLite
set TARGET_NAME=USQLite.uplugin
set OUTPUT_FOLDER=%~dp0\Plugins\USQLiteNew


setlocal 
echo Rebuild Plugin...

echo UE4_Install=%UE4_Install%
echo UAT=%UE4_Install%\%UE4_UAT%

if not exist "%UE4_Install%\%UE4_UAT%" goto Error_BatchFileInWrongLocation

call "%UE4_Install%\%UE4_UAT%" BuildPlugin -plugin=%TARGET_FOLDER%\%TARGET_NAME% -package=%OUTPUT_FOLDER%

rem ## Success!
goto Exit

:Error_BatchFileInWrongLocation
echo RunUAT.bat ERROR: The batch file does not appear to be located in the /Engine/Build/BatchFiles directory. 
set RUNUAT_EXITCODE=1
goto Exit_Failure


:Exit_Failure
echo BUILD FAILED
popd
exit /B %RUNUAT_EXITCODE%

:Exit
rem ## Restore original CWD in case we change it
popd
exit /B 0

