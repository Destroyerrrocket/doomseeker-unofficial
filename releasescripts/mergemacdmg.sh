#!/bin/bash
# Merges the Qt4 and Qt5 version of Doomseeker
# Usage:
#  ./mergemacdmg.sh <qt4path> <qt5path>

QT4=$1
QT5=$2
shift 2

CC="${QT4_CC}" CXX="${QT4_CXX}" ./makemacdmg.sh -DQT_QMAKE_EXECUTABLE="$QT4/bin/qmake" "$@" || exit
mkdir Qt4
mv Doomseeker*.dmg Qt4/
mv Doomseeker.app Qt4/

QT5PATH="$QT5" CC="${QT5_CC}" CXX="${QT5_CXX}" ./makemacdmg.sh "$@" || exit
mkdir Qt5
mv Doomseeker*.dmg Qt5/
mv Doomseeker.app Qt5/

cp -a Qt4/Doomseeker.app Doomseeker.app
cp -a Qt5/Doomseeker.app/Contents/Frameworks/Qt* Doomseeker.app/Contents/Frameworks/

function mergebin()
{
	cp Qt5/$1 $1.x86_64
	lipo $1 -thin ppc -output $1.ppc
	if [ ! -f $1.ppc ]
	then
		lipo $1 -thin ppc7400 -output $1.ppc
	fi
	lipo $1 -thin i386 -output $1.i386
	rm $1
	if [ -f $1.ppc ]
	then
		lipo -create -arch ppc $1.ppc -arch i386 $1.i386 -arch x86_64 $1.x86_64 -output $1
	else
		lipo -create -arch i386 $1.i386 -arch x86_64 $1.x86_64 -output $1
	fi
	rm $1.*
}

# Merge our binaries
mergebin Doomseeker.app/Contents/Frameworks/libwadseeker.dylib
mergebin Doomseeker.app/Contents/MacOS/doomseeker
mergebin Doomseeker.app/Contents/MacOS/updater
for i in `ls Doomseeker.app/Contents/MacOS/engines`
do
	mergebin Doomseeker.app/Contents/MacOS/engines/$i
done

# Copy new plugin directories
(cd Qt5; find Doomseeker.app/Contents/plugins -type d -exec mkdir -p "../{}" \;)

# Merge common plugins
for i in `comm -12 <(cd Qt4; find Doomseeker.app/Contents/plugins -name *.dylib | sort) <(cd Qt5; find Doomseeker.app/Contents/plugins -name *.dylib | sort)`
do
	mergebin $i
done
# Copy new plugins
for i in `comm -13 <(cd Qt4; find Doomseeker.app/Contents/plugins -name *.dylib | sort) <(cd Qt5; find Doomseeker.app/Contents/plugins -name *.dylib | sort)`
do
	cp Qt5/$i $i
done

./createmacdmg.sh `(cd Qt5;echo *.dmg)`
