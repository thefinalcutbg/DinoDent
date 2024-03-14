#!/bin/sh

APP="$PWD/../files/DinoDent.app" #the compiled bundle
INSTALLER="$PWD/../compiled/dinodent-macos.dmg"
QDEPLOY="$PWD/../../../Qt/6.5.3/macos/bin/macdeployqt" #macdeployqt location

defaults write $APP/Contents/Info.plist NSRequiresAquaSystemAppearance -bool yes #enforcing light theme

$QDEPLOY $APP -sign-for-notarization="Developer ID Application: Hristo Konstantinov" #deploying the app

#deleting the old dmg installer
Rm -R $INSTALLER

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
  $INSTALLER \
  "$PWD/../files/"

codesign --options runtime --timestamp -s "Developer ID Application: Hristo Konstantinov" $INSTALLER

xcrun notarytool submit $INSTALLER --keychain-profile "DeployProfile" --wait

xcrun stapler staple $INSTALLER

spctl --assess -vv --type install $INSTALLER

rm -R $APP
