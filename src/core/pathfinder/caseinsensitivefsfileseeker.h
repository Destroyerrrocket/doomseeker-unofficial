#ifndef id5DAEAB3C_C201_4B17_958E43B77B42E2FA
#define id5DAEAB3C_C201_4B17_958E43B77B42E2FA

#include "pathfinder/basefileseeker.h"

/**
 * @brief For case-insensitive file systems (like NTFS).
 */
class CaseInsensitiveFSFileSeeker : public BaseFileSeeker
{
	public:
		QString findFileInSpecifiedDirectory(const QString& fileName, const QString& dirPath);
};

#endif
