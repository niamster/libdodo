/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/ioString.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/toolsCode.h>

#include <iostream>

using namespace dodo;
using namespace io;
using namespace tools;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		filesystem::unlink("test.dat");

		io::string io;
		io.blockOffset = true;

		io.writeStreamString("!12345890#!!@\n");
		io.writeStreamString("!12345890-!!@\n");
		io.writeStreamString("!12345890@!!@\n");
		io.writeStreamString("!12345890$!!@\n");
		io.pos = 1;
		io.outSize = 14;
		io.inSize = 14;

		io::string io2 = io;

		io2.writeString("!12345890$!~@\n");

		filesystem::writeToFile("test.dat", io2);
		/**
		 * test.dat should contain
		 !12345890#!!@
		 !12345890$!~@
		 !12345890@!!@
		 !12345890$!!@
		 */

		dodoString str;

		io2.pos = 2;
		io2.readStreamString(str);
		cout << "\n\n~~" << str << "~~\n\n";

		io2.pos = 2;
		io2.readString(str);
		cout << "\n\n~~" << str << "~~\n\n";
	}
	catch (dodo::exception::basic ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
