#include <libdodo/baseEx.h>
#include <libdodo/logger.h>
#include <libdodo/ioFile.h>
#include <libdodo/ioStdio.h>

#include <iostream>

using namespace dodo;
using namespace std;

int main(int argc, char **argv)
{
	try
	{	
		logger log;

		ioFile *disk = new ioFile("./test.log", IOFILE_FILETYPE_REG_FILE, IOFILE_OPENMODE_APPEND);
		ioStdio *std = new ioStdio;
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
