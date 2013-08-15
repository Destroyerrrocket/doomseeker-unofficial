#include "basefileseeker.h"

#include "pathfinder/filesearchpath.h"
#include <QDir>

QString BaseFileSeeker::findFile(const QString& fileName, const QList<FileSearchPath>& paths)
{
	foreach (const FileSearchPath& candidate, paths)
	{
		QString result = findFileInPath(fileName, candidate);
		if (!result.isNull())
		{
			return result;
		}
	}
	return QString();
}

QString BaseFileSeeker::findFileInPath(const QString& fileName, const FileSearchPath& path)
{
	QString result = findFileInSpecifiedDirectory(fileName, path.path());
	if (!result.isNull())
	{
		return result;
	}

	if (path.isRecursive())
	{
		QDir dir(path.path());
		QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		foreach (const QString& subDir, subDirs)
		{
			FileSearchPath subSearchPath(dir.filePath(subDir));
			subSearchPath.setRecursive(true);
			QString result = findFileInPath(fileName, subSearchPath);
			if (!result.isNull())
			{
				return result;
			}
		}
	}
	return QString();
}
