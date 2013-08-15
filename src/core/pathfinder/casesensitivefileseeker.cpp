#include "casesensitivefileseeker.h"

#include "pathfinder/filesearchpath.h"
#include <QDir>
#include <QFileInfo>
#include <QStringList>

QString CaseSensitiveFileSeeker::findFileInSpecifiedDirectory(const QString& fileName,
	const QString& dirPath)
{
	QStringList filterList;
	filterList << fileName;
	QDir dir(dirPath);
	QFileInfoList fiList = dir.entryInfoList(filterList, QDir::Files);
	foreach (const QFileInfo& fileInfo, fiList)
	{
		QString candidate = fileInfo.fileName();
		if (candidate.compare(fileName, Qt::CaseInsensitive) == 0)
		{
			return fileInfo.absoluteFilePath();
		}
	}
	return QString();
}
