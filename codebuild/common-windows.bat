set CMAKE_ARGS=%*

set BUILDS_DIR=%TEMP%\builds
set INSTALL_DIR=%BUILDS_DIR%\install
mkdir %BUILDS_DIR%
mkdir %INSTALL_DIR%

@rem build aws-crt-cpp
mkdir %BUILDS_DIR%\aws-crt-cpp-build
cd %BUILDS_DIR%\aws-crt-cpp-build
git clone --branch -v0.4.0 https://github.com/awslabs/aws-crt-cpp.git
cmake %CMAKE_ARGS% -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_PREFIX_PATH="%INSTALL_DIR%" -DBUILD_DEPS=ON aws-crt-cpp || goto error
cmake --build . --target install --config RelWithDebInfo || goto error

@rem build SDK
mkdir %BUILDS_DIR%\aws-c-auth-build
cd %BUILDS_DIR%\aws-c-auth-build
cmake %CMAKE_ARGS% -DCMAKE_INSTALL_PREFIX="%INSTALL_DIR%" -DCMAKE_PREFIX_PATH="%INSTALL_DIR%" %CODEBUILD_SRC_DIR% || goto error
cmake --build . || goto error

goto :EOF

:error
echo Failed with error #%errorlevel%.
exit /b %errorlevel%
