#include "caseinsensitivefileseeker.h"

#include "pathfinder/filesearchpath.h"
#include "strings.h"
#include <QFileInfo>

QString CaseInsensitiveFileSeeker::findFileInSpecifiedDirectory(const QString& fileName,
	const QString& dirPath)
{
	QFileInfo file(Strings::combinePaths(dirPath, fileName));
	if (file.exists() && file.isFile())
	{
		return file.absoluteFilePath();
	}
	return QString();
}
