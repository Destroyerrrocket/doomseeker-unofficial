#!/bin/bash

SDK_10_4=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.4u.sdk
SDK_10_6=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.6.sdk

if [ -z $QT5PATH ]
then
	echo "Building Qt4 version."
	QT_VERSION=4
else
	echo "Building Qt5 version."
	QT_VERSION=5
fi

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

NUM_CPU_CORES=`system_profiler SPHardwareDataType | awk '/Total Number of Cores/ {print $5};'`

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
if [ -z $QT5PATH ]
then
	cmake ../../.. -DCMAKE_CXX_COMPILER=/usr/bin/g++-4.0 -DCMAKE_C_COMPILER=/usr/bin/gcc-4.0 $@
	cmake ../../.. -DCMAKE_OSX_ARCHITECTURES="ppc;i386" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.4 -DCMAKE_OSX_SYSROOT=${SDK_10_4} -DCMAKE_BUILD_TYPE=Release $@
else
	CMAKE_PREFIX_PATH=$QT5PATH cmake ../../.. $@
	CMAKE_PREFIX_PATH=$QT5PATH cmake ../../.. -DCMAKE_OSX_ARCHITECTURES="x86_64" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.6 -DCMAKE_OSX_SYSROOT=$SDK_10_6 -DCMAKE_BUILD_TYPE=Release $@
fi
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

if [ -d "${QTPATH}../hybridlib" ]
then
	# Blzut3 merged PowerPC Qt 4.7 and i386 Qt 4.8 into a single library which
	# works when compiled against 4.7. This pulls in the proper library.
	QTPATH=`echo $QTPATH | sed 's,/lib,/hybridlib,'`
	#QTPLPATH=`echo $QTPLPATH | sed 's,/plugins,/hybridplugins,'`
fi

echo "Qt Located: $QTPATH"
echo "Qt Plugins: $QTPLPATH"
echo

MODULES=`otool -L build/doomseeker | grep -o 'Qt[A-Za-z]\{1,\}.framework' | sed 's/.framework//'`
MODULES_COMMA=`echo $MODULES | sed 's/ /,/g'`

cp {build/,Doomseeker.app/Contents/MacOS/}doomseeker
cp {build/,Doomseeker.app/Contents/MacOS/}updater
cp {build/,Doomseeker.app/Contents/Frameworks/}libwadseeker.dylib
for i in $MODULES
do
	cp -a ${QTPATH}$i.framework Doomseeker.app/Contents/Frameworks/
	rm -f `find Doomseeker.app/Contents/Frameworks/$i.framework -name *_debug*`
	rm -rf `find Doomseeker.app/Contents/Frameworks/$i.framework -name Headers`
done
cp -a {${QTPLPATH},Doomseeker.app/Contents/}plugins
cp -R {build/,Doomseeker.app/Contents/MacOS/}engines
cp -R {build/,Doomseeker.app/Contents/MacOS/}translations
cp {${QTPLPATH},Doomseeker.app/Contents/MacOS/}translations/qt_pl.qm
cp {../../media/,Doomseeker.app/Contents/}Info.plist
cp {../../media/,Doomseeker.app/Contents/Resources/}icon-osx.icns
cp {../../media/,Doomseeker.app/Contents/Resources/}qt.conf

# Get a list of Qt plugins so that we can relink them.
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
for i in $MODULES
do
	install_name_tool -id {@executable_path/../,Doomseeker.app/Contents/}Frameworks/${i}.framework/Versions/$QT_VERSION/$i
	for j in `otool -L Doomseeker.app/Contents/Frameworks/${i}.framework/Versions/$QT_VERSION/$i | grep 'Qt[A-Za-z]*.framework' | tail -n +3 | awk '{print $1}'`
	do
		current=`echo $j | sed "s,$QTNTPATH,,"`
		install_name_tool -change {${QTNTPATH},@executable_path/../Frameworks/}$current Doomseeker.app/Contents/Frameworks/${i}.framework/Versions/$QT_VERSION/$i
	done
	for j in $RELINK_LIST
	do
		install_name_tool -change {${QTNTPATH},@executable_path/../Frameworks/}${i}.framework/Versions/$QT_VERSION/$i $j
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
