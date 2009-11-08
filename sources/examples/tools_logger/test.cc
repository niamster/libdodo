/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io;
using namespace tools;

using namespace std;

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    try {
        file::regular *disk = new file::regular;
        disk->open("./test.log", io::file::regular::OPEN_MODE_READ_WRITE_TRUNCATE);
        disk->append = true;

        stdio *std = new stdio;
        std->redirectToStderr(true);

        logger::instance().add(logger::LOG_LEVEL_ERROR, disk);
        logger::instance().add(logger::LOG_LEVEL_ERROR, NULL);
        logger::instance().add(logger::LOG_LEVEL_ERROR, std);
        logger::instance().add(logger::LOG_LEVEL_INFO, std);
        unsigned long infoDiskId = logger::instance().add(logger::LOG_LEVEL_INFO, disk);
        logger::instance().add(logger::LOG_LEVEL_WARNING, std);
        logger::instance().add(logger::LOG_LEVEL_DEBUG, std);

            ///or you can use a link what is more pretty
        logger &log = logger::instance();
        log.log(logger::LOG_LEVEL_INFO, "info message");
        log.log(logger::LOG_LEVEL_ERROR, "error message");
        log.remove(infoDiskId);
        log.log(logger::LOG_LEVEL_INFO, "another info message");
        log.log(logger::LOG_LEVEL_WARNING, "warning message");
        log.log(logger::LOG_LEVEL_DEBUG, "debug message");

            ///or you can use your own log
        logger mylog;
        mylog.add(logger::LOG_LEVEL_INFO, NULL);
        mylog.log(logger::LOG_LEVEL_INFO, "info message from mylog");

            ///and also redirect it to global
        mylog.forward = true;
        mylog.log(logger::LOG_LEVEL_INFO, "forwarded info message");

        delete disk;
        delete std;
    } catch (dodo::exception::basic &ex)   {
        cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
    }
    return 0;
}
