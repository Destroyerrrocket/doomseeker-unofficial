#pragma once

#include "UpdateInstaller.h"

/** Parses the command-line options to the updater binary. */
class UpdaterOptions
{
	public:
		UpdaterOptions();

		void parse(int argc, char** argv);

		UpdateInstaller::Mode mode;
		std::string installDir;
		std::string packageDir;
		std::string scriptPath;
		PLATFORM_PID waitPid;
		std::string logFile;
		/**
		 * @brief Program executed when updater.exe finishes.
		 *
		 * This program will be executed always, no matter if the
		 * update was successful or not. This overrides the
		 * "run-after-install" parameter in update script.
		 *
		 * runAfterInstallCmdArgs will also be passed to the program
		 * if specified.
		 *
		 * If update failed an additional "--update-failed <code>" argument
		 * pair will be passed to the program.
		 */
		std::string executable;
		/** Arguments passed to the program when installation is done. */
		std::list<std::string> runAfterInstallCmdArgs;
		bool showVersion;
		bool forceElevated;

	private:
		std::list<std::string> parseArgs(const std::string& args);
		void parseOldFormatArgs(int argc, char** argv);
		static void parseOldFormatArg(const std::string& arg, std::string* key, std::string* value);
};

