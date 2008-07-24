/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsMisc.h>
#include <libdodo/toolsCode.h>
#include <libdodo/ioFile.h>
#include <libdodo/baseEx.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/ioNetwork.h>

#include <iostream>

using namespace dodo;

using namespace std;

void
baseHandler(errorModuleEnum module, baseEx *ex, void *data)
{
	cout << "baseHandler: " << ex->baseErrstr << endl;
}

int main(int argc, char **argv)
{
	try
	{
		baseEx::setErrorHandler(ERRMODULE_IOFILE, &baseHandler, NULL);
#ifdef DL_EXT
		if (!baseEx::setErrorHandler("./module", NULL))
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
			tools::filesystem::unlink("some.file", false);
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
	catch (baseEx ex)
	{
		cout << endl << ex.baseErrno << endl << ex.baseErrstr  << ex.line << endl << ex.file << endl;
	}
	catch (exception ex)
	{
		cout << endl << ex.what() << endl;
	}

	return 0;
}

