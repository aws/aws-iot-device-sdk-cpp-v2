set CMAKE_ARGS=%*

set BUILDS_DIR=%TEMP%\builds
set INSTALL_DIR=%BUILDS_DIR%\install
mkdir %BUILDS_DIR%
mkdir %INSTALL_DIR%

@rem build SDK
mkdir %BUILDS_DIR%\aws-iot-device-sdk-cpp-v2-build
cd %CODEBUILD_SRC_DIR%
git submodule update --init --recursive
cd %BUILDS_DIR%\aws-iot-device-sdk-cpp-v2-build
cmake %CMAKE_ARGS% -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_PREFIX_PATH="%INSTALL_DIR%" -DBUILD_DEPS=ON %CODEBUILD_SRC_DIR% || goto error
cmake --build . || goto error

goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
