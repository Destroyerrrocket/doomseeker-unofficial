#!/bin/bash

# Get the version number
version_info=`grep --only-matching -E '[0-9.]{3,}|Beta|Alpha' ../src/version.cpp`
version="0.0"
tag=""
for i in $version_info
do
	if [ $i == "Beta" ]
	then
		tag="b"
	elif [ $i == "Alpha" ]
	then
		tag="a"
	else
		version=$i
	fi
done

echo "Version: $version"
echo "Tag: $tag"
echo

mkdir -p Doomseeker/Doomseeker.app/Contents/MacOS
mkdir Doomseeker/Doomseeker.app/Contents/Frameworks
mkdir Doomseeker/Doomseeker.app/Contents/Resources

# Build
mkdir Doomseeker/build
cd Doomseeker/build
cmake ../../.. -DMAC_ARCH_UNIVERSAL=ON -DMAC_SDK_10.4=ON -DCMAKE_BUILD_TYPE=Release
make -j 2
cd ..

cp {build/,Doomseeker.app/Contents/MacOS/}doomseeker
cp {build/,Doomseeker.app/Contents/Frameworks/}libwadseeker.dylib
cp -R {/Library/Frameworks/,Doomseeker.app/Contents/Frameworks/}QtCore.framework
cp -R {/Library/Frameworks/,Doomseeker.app/Contents/Frameworks/}QtGui.framework
cp -R {/Library/Frameworks/,Doomseeker.app/Contents/Frameworks/}QtNetwork.framework
cp -R {/Developer/Applications/Qt/,Doomseeker.app/Contents/}plugins
cp -R {build/,Doomseeker.app/Contents/MacOS/}engines
cp {../../media/,Doomseeker.app/Contents/}Info.plist
cp {../../media/,Doomseeker.app/Contents/Resources/}icon-osx.icns
cp {../../media/,Doomseeker.app/Contents/Resources/}qt.conf
rm -r Doomseeker.app/Contents/Frameworks/{QtCore,QtGui,QtNetwork}.framework/{*_debug.dSYM,Versions/4/Headers}
rm Doomseeker.app/Contents/Frameworks/{QtCore,QtGui,QtNetwork}.framework/{Versions/4/,}*_debug*

QTPLUGINS_LIST=''
for i in `ls Doomseeker.app/Contents/plugins`
do
	for j in `ls Doomseeker.app/Contents/plugins/${i}`
	do
		QTPLUGINS_LIST="$QTPLUGINS_LIST Doomseeker.app/Contents/plugins/${i}/${j}"
	done
done

install_name_tool -change `otool -L Doomseeker.app/Contents/MacOS/doomseeker | grep -m 1 --only-matching '[/@].*libwadseeker.dylib'` @executable_path/../Frameworks/libwadseeker.dylib Doomseeker.app/Contents/MacOS/doomseeker
for i in QtCore QtGui QtNetwork
do
	install_name_tool -id {@executable_path/../,Doomseeker.app/Contents/}Frameworks/${i}.framework/Versions/4/$i
	install_name_tool -change {,@executable_path/../Frameworks/}QtCore.framework/Versions/4/QtCore Doomseeker.app/Contents/Frameworks/${i}.framework/Versions/4/$i
	for j in `ls Doomseeker.app/Contents/{MacOS/{doomseeker,engines/*.so},Frameworks/libwadseeker.dylib}` $QTPLUGINS_LIST
	do
		install_name_tool -change {,@executable_path/../Frameworks/}${i}.framework/Versions/4/$i $j
	done
done

rm -r build

cd ..

# Create/Mount the image
IMAGENAME=Doomseeker-${version}${tag}.dmg
mkdir image
hdiutil create -size 100m -srcfolder Doomseeker -volname Doomseeker -fs HFS+ -format UDRW $IMAGENAME
hdiutil attach -readwrite -noverify -noautoopen $IMAGENAME -mountpoint image

mkdir image/.background
cp ../media/Doomseeker-DMG-Background.png image/.background/background.png
osascript makemacdmg.applescript

# Finalize
hdiutil detach ./image
hdiutil convert $IMAGENAME -format UDBZ -o tmp$IMAGENAME
mv {tmp,}$IMAGENAME
hdiutil internet-enable -yes $IMAGENAME

rm -r Doomseeker image
