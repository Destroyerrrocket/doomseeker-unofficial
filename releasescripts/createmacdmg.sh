#!/bin/bash

# Create/Mount the image
IMAGENAME=$1
mkdir image
hdiutil create -size 300m -srcfolder image -volname Doomseeker -fs HFS+ -format UDRW $IMAGENAME
hdiutil attach -readwrite -noverify -noautoopen $IMAGENAME -mountpoint image

mkdir image/.background
cp -a Doomseeker.app image/Doomseeker.app
cp ../media/Doomseeker-DMG-Background.png image/.background/background.png
osascript makemacdmg.applescript

# Finalize
hdiutil detach ./image
hdiutil convert $IMAGENAME -format UDBZ -o tmp$IMAGENAME
mv {tmp,}$IMAGENAME
hdiutil internet-enable -yes $IMAGENAME

rm -r image

