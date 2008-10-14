/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/toolsLogger.h>
#include <libdodo/ioFile.h>
#include <libdodo/ioStdio.h>

#include <iostream>

using namespace dodo;
using namespace io;
using namespace tools;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		file::regular *disk = new file::regular;
		disk->open("./test.log", file::REGULAR_OPENMODE_APPEND);

		stdio *std = new stdio;
		std->redirectToStderr(true);

		logger::getInstance().add(LOGGER_ERROR, disk);
		logger::getInstance().add(LOGGER_ERROR, NULL);
		logger::getInstance().add(LOGGER_ERROR, std);
		logger::getInstance().add(LOGGER_INFO, std);
		unsigned long infoDiskId = logger::getInstance().add(LOGGER_INFO, disk);
		logger::getInstance().add(LOGGER_WARNING, std);
		logger::getInstance().add(LOGGER_DEBUG, std);

		///or you can use a link what is more pretty
		logger &log = logger::getInstance();
		log.log(LOGGER_INFO, "info message");
		log.log(LOGGER_ERROR, "error message");
		log.remove(infoDiskId);
		log.log(LOGGER_INFO, "another info message");
		log.log(LOGGER_WARNING, "warning message");
		log.log(LOGGER_DEBUG, "debug message");

		///or you can use your own log
		logger mylog;
		mylog.add(LOGGER_INFO, NULL);
		mylog.log(LOGGER_INFO, "info message from mylog");

		///and also redirect it to global
		mylog.forward = true;
		mylog.log(LOGGER_INFO, "forwarded info message");

		delete disk;
		delete std;
	}
	catch (dodo::exception::basic ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << "\t" << ex.file << endl;
	}
	return 0;
}
