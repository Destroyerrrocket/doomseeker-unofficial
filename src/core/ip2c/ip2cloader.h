#ifndef idBE97C916_2BFB_4C8C_B530CA666606FA7D
#define idBE97C916_2BFB_4C8C_B530CA666606FA7D

#include <QObject>

class IP2CLoader : public QObject
{
	Q_OBJECT

	public:
		IP2CLoader();
		~IP2CLoader();

		void load();
		void update();

	signals:
		void downloadProgress(qint64 current, qint64 total);
		void finished();

	private:
		class PrivData;

		PrivData* d;

		void ip2cJobsFinished();
		void ip2cParseDatabase();
		void ip2cStartUpdate();

	private slots:
		void ip2cFinishUpdate(const QByteArray& downloadedData);
		void ip2cFinishedParsing(bool bSuccess);
};

#endif
