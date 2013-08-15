#ifndef id05405E76_A2CE_42DA_B784CB577350685F
#define id05405E76_A2CE_42DA_B784CB577350685F

#include <QList>
#include <QString>

class FileSearchPath;

class BaseFileSeeker
{
	public:
		virtual ~BaseFileSeeker() {}

		QString findFile(const QString& fileName, const QList<FileSearchPath>& paths);
		QString findFileInPath(const QString& fileName, const FileSearchPath& path);
		virtual QString findFileInSpecifiedDirectory(const QString& fileName, const QString& dirPath) = 0;
};

#endif // header
