@echo off
echo ========================================
echo ESP8266 Afterburner - GitHub Deployment
echo ========================================
echo.

echo Step 1: Building the project...
call npm run build
if %errorlevel% neq 0 (
    echo Build failed! Please check for errors.
    pause
    exit /b 1
)

echo.
echo Step 2: Deploying to GitHub Pages...
call npm run deploy
if %errorlevel% neq 0 (
    echo Deployment failed! Please check your GitHub repository settings.
    pause
    exit /b 1
)

echo.
echo ========================================
echo Deployment completed successfully!
echo ========================================
echo.
echo Your website should be available at:
echo https://yourusername.github.io/afterburner-presentation
echo.
echo Note: It may take a few minutes for changes to appear.
echo.
pause
