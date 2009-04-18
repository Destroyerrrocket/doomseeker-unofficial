#include "pathfinder.h"
#include <QDir>
#include <QFileInfo>

PathFinder::PathFinder(Config* cfg)
{
	config = cfg;
}

QString PathFinder::findWad(const QString& fileName)
{
	if (config == NULL)
		return QString();

	SettingsData* setting;
	setting = config->setting("WadPaths");
	QStringList strList = setting->string().split(";", QString::SkipEmptyParts);

	for (int i = 0; i < strList.count(); ++i)
	{
		QFileInfo file(strList[i] + QDir::separator() + fileName);
		if (file.exists() && file.isFile())
			return file.absoluteFilePath();
	}

	return QString();
}
