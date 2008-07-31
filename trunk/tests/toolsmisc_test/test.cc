/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>
#include <libdodo/ioFile.h>
#include <libdodo/exceptionBasic.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetwork.h>

#include <iostream>

using namespace dodo;

using namespace std;
using namespace io;

void
baseHandler(int module, dodo::exception::basic *ex, void *data)
{
	cout << "baseHandler: " << ex->baseErrstr << endl;
}

int main(int argc, char **argv)
{
	try
	{
		dodo::exception::basic::setErrorHandler(dodo::exception::ERRMODULE_IOFILE, &baseHandler, NULL);
#ifdef DL_EXT
		if (!dodo::exception::basic::setErrorHandler("./module", NULL))
			cout << "WTF";
#endif

		try
		{
			tools::filesystem::unlink("some.file", false);
		}
		catch(...)
		{
		}
		
		try
		{
			file f("some.file", FILE_FILETYPE_REG_FILE, FILE_OPENMODE_READ_ONLY);
		}
		catch(...)
		{
		}

		dodoString t = "abcddF";
		tools::string::replace("cd", "WW", t);
		cout << t << endl;

		dodoString rnd = tools::misc::stringRandom(12);
		cout << tools::code::MD5Hex(rnd) << endl;

		//tools::misc::mail("niam", "test", "test");
		//tools::misc::mail("127.0.0.1", 25, "niam@niam.mu", "niam@niam.mu", "test", "test", "niam", "niam");
	}
	catch (dodo::exception::basic ex)
	{
		cout << endl << ex.baseErrno << endl << ex.baseErrstr  << ex.line << endl << ex.file << endl;
	}
	catch (std::exception &ex)
	{
		cout << endl << ex.what() << endl;
	}

	return 0;
}

