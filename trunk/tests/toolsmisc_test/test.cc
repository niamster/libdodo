/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
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
		dodo::exception::basic::setErrorHandler(dodo::exception::ERRMODULE_IOFILEREGULAR, &baseHandler, NULL);
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
			file::regular f;
			f.open("some.file", file::REGULAR_OPENMODE_READ_ONLY);
		}
		catch(...)
		{
		}

		dodoString t = "abcddF";
		tools::string::replace("cd", "WW", t);
		cout << t << endl;

		dodoString rnd = tools::misc::stringRandom(12);
		cout << tools::code::MD5Hex(rnd) << endl;
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

