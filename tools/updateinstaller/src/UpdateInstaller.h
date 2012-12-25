#pragma once

#include "Platform.h"
#include "FileUtils.h"
#include "UpdateScript.h"

#include <list>
#include <string>
#include <map>

class UpdateObserver;

/** Central class responsible for installing updates,
  * launching an elevated copy of the updater if required
  * and restarting the main application once the update
  * is installed.
  */
class UpdateInstaller
{
	public:
		// This enum requires integer assignments because
		// it's passed as an argument to another program.
		// Values must be consistent.
		enum UpdateErrorCode
		{
			UEC_Ok = 0,
			UEC_UnableToReadUpdateScript = 1,
			UEC_NoInstallationDirectorySpecified = 2,
			UEC_UnableToDeterminePathOfUpdater = 3,
			UEC_GeneralFailure = 10000
		};

		enum Mode
		{
			Setup,
			Main
		};

		UpdateInstaller();
		void setArgsForRunAfterInstall(const std::list<std::string>& args)
		{
			m_runAfterInstallArgs = args;
		}
		/**
		 * @brief Executable to start when update is done.
		 *
		 * If "run-after-install" is defined in the script then this
		 * method will override this value. "run-after-install"
		 * is used only if "exec" is set to an empty string.
		 */
		void setExecForRunAfterInstall(const std::string& exec)
		{
			m_runAfterInstallExec = exec;
		}
		void setInstallDir(const std::string& path);
		void setPackageDir(const std::string& path);
		void setBackupDir(const std::string& path);
		void setMode(Mode mode);
		void setScript(UpdateScript* script);
		void setWaitPid(PLATFORM_PID pid);
		void setForceElevated(bool elevated);

		void setObserver(UpdateObserver* observer);

		void run() throw ();

		/**
		 * @brief Restarts main program.
		 *
		 * Program path:
		 * - If setExecForRunAfterInstall() was set to a non-empty string
		 *   then it's used as a path to the program.
		 * - Otherwise path is read from "run-after-install" value
		 *   from install script.
		 * - If none are present then no program is started.
		 *
		 * Program arguments are read from setArgsForRunAfterInstall().
		 *
		 * @param errorCode
		 *     This is passed to the executed program if set to something else
		 *     than UEC_Ok. The code is passed as "--update-failed <arg>"
		 *     argument pair, where <arg> is int value of UpdateErrorCode.
		 */
		void restartMainApp(UpdateErrorCode errorCode = UEC_Ok);

	private:
		void cleanup();
		void revert();
		void removeBackups();
		bool checkAccess();

		void installFiles();
		void uninstallFiles();
		void installFile(const UpdateScriptFile& file);
		void backupFile(const std::string& path);
		void reportError(const std::string& error);
		void postInstallUpdate();

		std::list<std::string> updaterArgs() const;
		std::string friendlyErrorForError(const FileUtils::IOException& ex) const;

		Mode m_mode;
		std::list<std::string> m_runAfterInstallArgs;
		std::string m_runAfterInstallExec;
		std::string m_installDir;
		std::string m_packageDir;
		std::string m_backupDir;
		PLATFORM_PID m_waitPid;
		UpdateScript* m_script;
		UpdateObserver* m_observer;
		std::map<std::string,std::string> m_backups;
		bool m_forceElevated;
};

