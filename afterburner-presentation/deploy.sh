#!/bin/bash

echo "========================================"
echo "ESP8266 Afterburner - GitHub Deployment"
echo "========================================"
echo

echo "Step 1: Building the project..."
npm run build
if [ $? -ne 0 ]; then
    echo "Build failed! Please check for errors."
    exit 1
fi

echo
echo "Step 2: Deploying to GitHub Pages..."
npm run deploy
if [ $? -ne 0 ]; then
    echo "Deployment failed! Please check your GitHub repository settings."
    exit 1
fi

echo
echo "========================================"
echo "Deployment completed successfully!"
echo "========================================"
echo
echo "Your website should be available at:"
echo "https://yourusername.github.io/afterburner-presentation"
echo
echo "Note: It may take a few minutes for changes to appear."
echo
