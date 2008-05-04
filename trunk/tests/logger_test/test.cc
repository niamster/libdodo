#include <libdodo/baseEx.h>
#include <libdodo/logger.h>
#include <libdodo/ioFile.h>
#include <libdodo/ioStdio.h>

#include <iostream>

using namespace dodo;
using namespace io;

using namespace std;

int main(int argc, char **argv)
{
	try
	{	
		logger log;

		file *disk = new file("./test.log", FILE_FILETYPE_REG_FILE, FILE_OPENMODE_APPEND);
		stdio *std = new stdio;
		std->redirectToSTDErr(true);

		log.add(LOGGER_ERROR, disk);
		log.add(LOGGER_ERROR, NULL);
		log.add(LOGGER_ERROR, std);
		log.add(LOGGER_INFO, std);
		unsigned long infoDiskId = log.add(LOGGER_INFO, disk);
		log.add(LOGGER_WARNING, std);
		log.add(LOGGER_DEBUG, std);

		log.log(LOGGER_INFO, "info message");
		log.log(LOGGER_ERROR, "error message");
		log.remove(infoDiskId);
		log.log(LOGGER_INFO, "another info message");
		log.log(LOGGER_WARNING, "warning message");
		log.log(LOGGER_DEBUG, "debug message");

		delete disk;
		delete std;
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << "\t" << ex.file << endl;
	}
	return 0;
}
