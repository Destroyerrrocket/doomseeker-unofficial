#ifndef __PATHFINDER_H_
#define __PATHFINDER_H_

#include "sdeapi/config.hpp"

class PathFinder
{
	private:
		Config* config;

	public:
		PathFinder(Config*);

		QString findWad(const QString& fileName);

};

#endif
