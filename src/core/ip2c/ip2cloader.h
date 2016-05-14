#ifndef idBE97C916_2BFB_4C8C_B530CA666606FA7D
#define idBE97C916_2BFB_4C8C_B530CA666606FA7D

#include "dptr.h"

#include <QObject>

class IP2CLoader : public QObject
{
	Q_OBJECT

	public:
		IP2CLoader(QObject *parent = NULL);
		~IP2CLoader();

		void load();

	public slots:
		void update();

	signals:
		void downloadProgress(qint64 current, qint64 total);
		void finished();

	private:
		DPtr<IP2CLoader> d;

		void ip2cJobsFinished();
		void ip2cParseDatabase();

	private slots:
		void ip2cFinishUpdate(const QByteArray& downloadedData);
		void ip2cFinishedParsing(bool bSuccess);
		void onUpdateNeeded(int status);
};

#endif
