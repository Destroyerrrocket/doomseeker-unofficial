#include "caseinsensitivefsfileseeker.h"

#include "pathfinder/filesearchpath.h"
#include "strings.h"
#include <QFileInfo>

QString CaseInsensitiveFSFileSeeker::findFileInSpecifiedDirectory(const QString& fileName,
	const QString& dirPath)
{
	QFileInfo file(Strings::combinePaths(dirPath, fileName));
	if (file.exists() && file.isFile())
	{
		return file.absoluteFilePath();
	}
	return QString();
}
