#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <QtGlobal>

#define VERSION		"0.5 Beta"

#define READINT32(pointer) ((quint32((quint8)(*pointer))) | (quint32(quint8(*(pointer+1)))<<8) | (quint32(quint8(*(pointer+2)))<<16) | (quint32(quint8(*(pointer+3)))<<24))
#define READINT16(pointer) ((quint16((quint8)(*pointer))) | (quint16(quint8(*(pointer+1)))<<8))
#define READBIGINT16(pointer) ((quint16((quint8)(*pointer))<<8) | (quint16(quint8(*(pointer+1)))))
#define READINT8(pointer) ((quint8)(*pointer))

#define WRITEINT32_DIRECT(integer) (quint8)(integer&0xFF),(quint8)((integer>>8)&0xFF),(quint8)((integer>>16)&0xFF),(quint8)((integer>>24)&0xFF)
#define WRITEINT16_DIRECT(integer) (quint8)(integer&0xFF),(quint8)((integer>>8)&0xFF)
#define WRITEINT8_DIRECT(integer) (quint8)(integer&0xFF)

#define WRITEINT32(pointer, integer) *pointer = (quint8)(integer&0xFF);*(pointer+1) = (quint8)((integer>>8)&0xFF);*(pointer+2) = (quint8)((integer>>16)&0xFF);*(pointer+3) = (quint8)((integer>>24)&0xFF);
#define WRITEINT16(pointer, integer) *pointer = (quint8)(integer&0xFF);*(pointer+1) = (quint8)((integer>>8)&0xFF);
#define WRITEINT8(pointer, integer) *pointer = (quint8)(integer&0xFF);

// Now we set it up so symbols are properly exported/imported on Windows
#ifdef Q_OS_WIN32
#ifdef MODE_MAIN
#define MAIN_EXPORT		__declspec(dllexport)
#define PLUGIN_EXPORT	__declspec(dllimport)
#else
#define MAIN_EXPORT		__declspec(dllimport)
#define PLUGIN_EXPORT	__declspec(dllexport)
#endif
#else
#define MAIN_EXPORT
#define PLUGIN_EXPORT
#endif

#endif /* __GLOBAL_H__ */
