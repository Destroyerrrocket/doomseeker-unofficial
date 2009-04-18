#include "pathfinder.h"

PathFinder::PathFinder(Config* cfg)
{
	config = cfg;
}

QString PathFinder::findFile(QString fileName)
{
	if (config == NULL)
		return QString();
}
