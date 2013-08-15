#ifndef idD5CA37A7_5FD3_4151_8316AB0B1F9974E5
#define idD5CA37A7_5FD3_4151_8316AB0B1F9974E5

#include <QString>
#include <QVariant>

class FileSearchPath
{
	public:
		static FileSearchPath fromVariant(const QVariant& var);
		static QList<FileSearchPath> fromVariantList(const QVariantList& collection);
		static QVariantList toVariantList(const QList<FileSearchPath>& collection);

		FileSearchPath();
		FileSearchPath(const QString& path);

		bool isRecursive() const
		{
			return recursive_;
		}

		bool isValid() const;
		QVariant toVariant() const;

		const QString& path() const
		{
			return path_;
		}

		void setPath(const QString& path)
		{
			path_ = path;
		}

		void setRecursive(bool b)
		{
			recursive_ = b;
		}

	private:
		QString path_;
		bool recursive_;

};

#endif
