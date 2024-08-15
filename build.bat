@echo off
setlocal enabledelayedexpansion

set "show_help=0"
for %%a in (%*) do (
    if "%%a"=="help" (
        set "show_help=1"
        goto show_help_label
    )
)

:build_process
cd cmake

if not exist "Makefile" (
    cmake .
)

set CMAKE_OPTIONS=
set TARGETS=

set "TEMP_ARG="
for %%a in (%*) do (
    set "CUR_ARG=%%a"
    if defined TEMP_ARG (
        set "CUR_ARG=!TEMP_ARG!=!CUR_ARG!"
        set "TEMP_ARG="
    )
    echo !CUR_ARG! | find "=" > nul
    if not errorlevel 1 (
        set CMAKE_OPTIONS=!CMAKE_OPTIONS! !CUR_ARG!
    ) else (
        echo !CUR_ARG! | find "-D" > nul
        if not errorlevel 1 (
            set "TEMP_ARG=!CUR_ARG!"
        ) else (
            set TARGETS=!TARGETS! !CUR_ARG!
        )
    )
)

echo Debug: CMAKE_OPTIONS=!CMAKE_OPTIONS!
echo Debug: TARGETS=!TARGETS!

if not "!CMAKE_OPTIONS!" == "" (
    cmake . !CMAKE_OPTIONS!
)

if not "!TARGETS!" == "" (
    for %%t in (!TARGETS!) do (
        cmake --build . --target %%t
    )
) else (
    cmake --build .
)

echo !TARGETS! | find " clean " > nul
if not errorlevel 1 (
    cmake --build . --target clean
)

goto :EOF

:show_help_label
echo Available targets:
echo  AnyTypeTest         - Build the AnyTypeTest target
echo  clean               - Clean the build directory
echo.
echo Examples:
echo  build.bat AnyTypeTest
echo  build.bat clean
echo  build.bat -DUSE_RAW_POINTER=ON AnyTypeTest
goto :EOF