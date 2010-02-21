cmake_minimum_required( VERSION 2.4 )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")

set(LZMA_FILES
	C/Archive/7z/7zDecode.c
	C/Archive/7z/7zExtract.c
	C/Archive/7z/7zHeader.c
	C/Archive/7z/7zIn.c
	C/Archive/7z/7zItem.c
	C/7zBuf.c
	C/7zCrc.c
	C/7zStream.c
	C/Alloc.c
	C/Bcj2.c
	C/Bra86.c
	C/LzFind.c
	C/LzFindMt.c
	C/LzmaDec.c
	C/LzmaEnc.c
	C/LzmaLib.c
)

add_library(lzma ${LZMA_FILES})
