#ifndef id9C0844AD_C55F_4877_8BBACDF0498131F5
#define id9C0844AD_C55F_4877_8BBACDF0498131F5

#include "pathfinder/basefileseeker.h"

/**
 * @brief For case-sensitive file systems (like ext).
 */
class CaseSensitiveFileSeeker : public BaseFileSeeker
{
	public:
		QString findFileInSpecifiedDirectory(const QString& fileName, const QString& dirPath);
};

#endif
