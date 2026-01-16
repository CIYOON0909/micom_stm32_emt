@echo off
REM CMD /C ${ProjDirPath}\post_make_packfile.cmd ${ProjDirPath} 0x5800

echo.
echo run post_make_packfile.cmd
echo.


@set PROJ_DIR=%1
@set MAIN_APP_START_ADDRESS=%2
@set MODEL_NAME=CR3


REM echo %PROJ_DIR%

IF "%MODEL_NAME%"=="CR3" (
	IF NOT EXIST %PROJ_DIR%\BOOTLOADER\MICOM_STM32_BOOTLOADER.bin echo "Bootloader image file not exist !!!" : EXIT

	IF NOT EXIST %PROJ_DIR%\MAIN_APPL\MICOM_STM32_MAIN_APPL.bin echo "Main_app image file not exist !!!" : EXIT
) ELSE (
	IF NOT EXIST %PROJ_DIR%\BOOTLOADER\%MODEL_NAME%_MICOM_STM32_BOOTLOADER.bin echo "Bootloader image file not exist !!!" : EXIT

	IF NOT EXIST %PROJ_DIR%\MAIN_APPL\%MODEL_NAME%_MICOM_STM32_MAIN_APPL.bin echo "Main_app image file not exist !!!" : EXIT
)

%PROJ_DIR%\fwconv_stm32.exe %PROJ_DIR%\%TARGET_BNAME% %MAIN_APP_START_ADDRESS% %MODEL_NAME%


REM echo %1
REM echo %2