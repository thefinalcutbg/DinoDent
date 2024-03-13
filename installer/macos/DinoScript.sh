#!/bin/sh

APP="$PWD/../files/DinoDent.app" #the compiled bundle

QDEPLOY="$PWD/../../../Qt/6.5.3/macos/bin/macdeployqt" #macdeployqt location

$QDEPLOY $APP #deploying the app

defaults write $APP/Contents/Info.plist NSRequiresAquaSystemAppearance -bool yes #enforcing light theme

codesign --force --deep --sign - $APP #signing

#creating dmg installer using create-dmg
create-dmg \
  --volname "DinoDent Installer" \
  --volicon "icon.icns" \
  --background "background.png" \
  --window-pos 200 110 \
  --window-size 605 360 \
  --icon-size 90 \
  --icon "DinoDent.app" 160 150 \
  --app-drop-link 435 150 \
  --hide-extension "DinoDent.app" \
  "$PWD/../compiled/dinodent-macos.dmg" \
  "$PWD/../files/"

rm -R $APP