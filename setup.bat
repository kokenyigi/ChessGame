@echo off
setlocal

set "buildDirectory=build"

set "shouldBuildServer=0"
set "shouldBuildClient=0"

if not "%~1"=="" (
    if "%~1"=="server" (
        set "shouldBuildServer=1"
    ) else if "%~1"=="client" (
        set "shouldBuildClient=1"
    ) else (
        echo Invalid Argument: %~1
        exit /b 1
    )
) else (
    set "shouldBuildClient=1"
    set "shouldBuildServer=1"
)

if not exist "%buildDirectory%" (
    mkdir "%buildDirectory%"
)

cd /d "%buildDirectory%"

if "%shouldBuildClient%"=="1" (
    if "%shouldBuildServer%"=="1" (
        cmake .. -G "Ninja" -DBUILD_CLIENT=ON -DBUILD_SERVER=ON
    ) else (
        cmake .. -G "Ninja" -DBUILD_CLIENT=ON -DBUILD_SERVER=OFF
    )
) else (
    if "%shouldBuildServer%"=="1" (
        cmake .. -G "Ninja" -DBUILD_CLIENT=OFF -DBUILD_SERVER=ON
    )
)

if errorlevel 1 (
    echo CMake configuration failed.
    cd ..
    exit /b 1
)

cmake --build .

if errorlevel 1 (
    echo Build failed.
    cd ..
    exit /b 1
)

cd ..

endlocal