#include "filesearchpath.h"

FileSearchPath::FileSearchPath()
{
	recursive_ = false;
}

FileSearchPath::FileSearchPath(const QString& path)
{
	path_ = path;
	recursive_ = false;
}

FileSearchPath FileSearchPath::fromVariant(const QVariant& var)
{
	FileSearchPath result;
	QVariantList list = var.toList();
	if (list.size() >= 1 && list.size() <= 2)
	{
		result.setPath(list[0].toString());
		if (list.size() == 2)
		{
			result.setRecursive(list[1].toBool());
		}
	}
	return result;
}

QList<FileSearchPath> FileSearchPath::fromVariantList(const QVariantList& collection)
{
	QList<FileSearchPath> result;
	foreach (const QVariant& variant, collection)
	{
		result << fromVariant(variant);
	}
	return result;
}

bool FileSearchPath::isValid() const
{
	return path_.isNull();
}

QVariant FileSearchPath::toVariant() const
{
	QVariantList var;
	var << path_;
	var << recursive_;
	return var;
}

QVariantList FileSearchPath::toVariantList(const QList<FileSearchPath>& collection)
{
	QVariantList result;
	foreach (const FileSearchPath& path, collection)
	{
		result << path.toVariant();
	}
	return result;
}
