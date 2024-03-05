#!/bin/sh

APP="$PWD/../files/DinoDent.app" #the compiled bundle

defaults write $APP/Contents/Info.plist NSRequiresAquaSystemAppearance -bool yes #enforcing light theme

QDEPLOY="$PWD/../../../Qt/6.5.3/macos/bin/macdeployqt" #macdeployqt location

$QDEPLOY $APP #deploying the app

codesign --force --deep --sign - $APP #signing

#creating dmg installer using create-dmg
create-dmg \
  --volname "DinoDent Installer" \
  --volicon "icon.icns" \
  --background "background.png" \
  --window-pos 200 110 \
  --window-size 600 355 \
  --icon-size 90 \
  --icon "DinoDent.app" 160 150 \
  --app-drop-link 435 150 \
  --hide-extension "DinoDent.app" \
  "$PWD/../compiled/dinodent-arm64.dmg" \
  "$PWD/../files/"
