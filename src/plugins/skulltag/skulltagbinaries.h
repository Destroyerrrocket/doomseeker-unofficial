//------------------------------------------------------------------------------
// skulltagbinaries.h
//------------------------------------------------------------------------------
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
// 02110-1301, USA.
//
//------------------------------------------------------------------------------
// Copyright (C) 2010 "Zalewa" <zalewapl@gmail.com>
//------------------------------------------------------------------------------
#ifndef __SKULLTAG_BINARIES_H_
#define __SKULLTAG_BINARIES_H_

#include <QProgressDialog>
#include <QNetworkReply>

#include "serverapi/binaries.h"

class EnginePlugin;
class SkulltagServer;
class QDir;

class SkulltagBinaries : public Binaries
{
	public:
		SkulltagBinaries(const SkulltagServer* server);

		QString					configKeyServerBinary() const;

		/**
		 *	If the parent Server is a normal server simple path to executable
		 *	file is returned. If this is a testing server, a shell script is
		 *	created	if necessary and a path to this shell script s returned.
		 */
		QString					clientBinary(Message& message) const;
		QString					clientWorkingDirectory(Message& message) const;

		const EnginePlugin*		plugin() const;

	protected:
		const SkulltagServer*	server;

		bool					downloadTestingBinaries(const QDir &destination) const;
		/**
		 *	Creates Unix .sh file or Windows .bat file to
		 *	launch client for parent server. Returns true if the file
		 *	already exists.
		 *	@param versionDir - convenience parameter. This is the directory
		 *		where testing package was unpacked. This path was
		 *		already created in clientBinary() method so let's reuse it.
		 *	@param [out] fullPathToFile - path to created script file
		 *	@param [out] error - error if return == false
		 *	@return false if fail
		 */
		bool					spawnTestingBatchFile(const QString& versionDir, QString& fullPathToFile, Message& message) const;
};

class TestingProgressDialog : public QProgressDialog
{
	Q_OBJECT

	public:
		TestingProgressDialog(const QUrl& url);

		const QByteArray	&data() const { return downloadedFileData; }
		const QString		&filename() const { return downloadedFilename; }

	private slots:
		void	abort();
		void	downloadFinished();
		void	downloadProgress(qint64 value, qint64 max);


	private:
		QByteArray	downloadedFileData;
		QString		downloadedFilename;
		QNetworkAccessManager networkAccessManager;
		QNetworkReply* pNetworkReply;

		void getUrl(const QUrl& url);
};

#endif
