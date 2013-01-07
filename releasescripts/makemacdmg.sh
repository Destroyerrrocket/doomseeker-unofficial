#!/bin/bash

# Get the version number
version_info=`grep --only-matching -E '([0-9]+\.[0-9]+)|Beta|Alpha' ../src/core/versiondefs.h`
version=""
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
		if [ "$version" == "" ]
		then
			version=$i
		fi
	fi
done
if [ "$version" == "" ]
then
	version="0.0"
fi

NUM_CPU_CORES=`system_profiler SPHardwareDataType | awk '/Total Number Of Cores/ {print $5};'`

echo "Version: $version"
echo "Tag: $tag"
echo "Building with $NUM_CPU_CORES jobs!"
echo

mkdir -p Doomseeker/Doomseeker.app/Contents/MacOS
mkdir Doomseeker/Doomseeker.app/Contents/Frameworks
mkdir Doomseeker/Doomseeker.app/Contents/Resources

# Build
mkdir Doomseeker/build
cd Doomseeker/build
cmake ../../.. -DMAC_ARCH_UNIVERSAL=ON -DMAC_SDK_10.4=ON -DCMAKE_BUILD_TYPE=Release $@
# Run CMake twice since it will try to reconfigure itself... grr...
cmake ../../.. -DCMAKE_BUILD_TYPE=Release $@
make -j $NUM_CPU_CORES
cd ..

# Detect Qt installation
QTCORE_STRING=`otool -L build/doomseeker | grep QtCore.framework`
QTPATH=`echo $QTCORE_STRING | sed 's,QtCore.framework.*,,'`
QTNTPATH=$QTPATH
QTPLPATH=${QTPATH}../
if [ -z "$QTPATH" ]
then
	QTPATH=/Library/Frameworks/
	QTPLPATH=/Developer/Applications/Qt/
fi
echo "Qt Located: $QTPATH"
echo "Qt Plugins: $QTPLPATH"
echo

cp {build/,Doomseeker.app/Contents/MacOS/}doomseeker
cp {build/,Doomseeker.app/Contents/MacOS/}updater
cp {build/,Doomseeker.app/Contents/Frameworks/}libwadseeker.dylib
cp -R {${QTPATH},Doomseeker.app/Contents/Frameworks/}QtCore.framework
cp -R {${QTPATH},Doomseeker.app/Contents/Frameworks/}QtGui.framework
cp -R {${QTPATH},Doomseeker.app/Contents/Frameworks/}QtNetwork.framework
cp -R {${QTPATH},Doomseeker.app/Contents/Frameworks/}QtXml.framework
cp -R {${QTPLPATH},Doomseeker.app/Contents/}plugins
cp -R {build/,Doomseeker.app/Contents/MacOS/}engines
cp -R {build/,Doomseeker.app/Contents/MacOS/}translations
cp {${QTPLPATH},Doomseeker.app/Contents/MacOS/}translations/qt_pl.qm
cp {../../media/,Doomseeker.app/Contents/}Info.plist
cp {../../media/,Doomseeker.app/Contents/Resources/}icon-osx.icns
cp {../../media/,Doomseeker.app/Contents/Resources/}qt.conf
rm -rf Doomseeker.app/Contents/Frameworks/{QtCore,QtGui,QtNetwork,QtXml}.framework/{*_debug.dSYM,Versions/4/Headers}
rm -f Doomseeker.app/Contents/Frameworks/{QtCore,QtGui,QtNetwork,QtXml}.framework/{Versions/4/,}*_debug*

QTPLUGINS_LIST=''
for i in `ls Doomseeker.app/Contents/plugins`
do
	for j in `ls Doomseeker.app/Contents/plugins/${i}`
	do
		QTPLUGINS_LIST="$QTPLUGINS_LIST Doomseeker.app/Contents/plugins/${i}/${j}"
	done
done

RELINK_LIST="`ls Doomseeker.app/Contents/{MacOS/{doomseeker,engines/*.so},Frameworks/libwadseeker.dylib}` $QTPLUGINS_LIST"
for i in $RELINK_LIST
do
	if [ "`otool -L $i | grep libwadseeker.dylib`" ]
	then
		install_name_tool -change `otool -L $i | grep -m 1 --only-matching '[/@].*libwadseeker.dylib'` @executable_path/../Frameworks/libwadseeker.dylib $i
	fi
done
for i in QtCore QtGui QtNetwork
do
	install_name_tool -id {@executable_path/../,Doomseeker.app/Contents/}Frameworks/${i}.framework/Versions/4/$i
	install_name_tool -change {${QTNTPATH},@executable_path/../Frameworks/}QtCore.framework/Versions/4/QtCore Doomseeker.app/Contents/Frameworks/${i}.framework/Versions/4/$i
	for j in $RELINK_LIST
	do
		install_name_tool -change {${QTNTPATH},@executable_path/../Frameworks/}${i}.framework/Versions/4/$i $j
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

mv Doomseeker/Doomseeker.app Doomseeker.app
rm -r Doomseeker image
