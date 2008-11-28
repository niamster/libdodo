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

		io.writeStream("!12345890#!!@\n");
		io.writeStream("!12345890-!!@\n");
		io.writeStream("!12345890@!!@\n");
		io.writeStream("!12345890$!!@\n");

		io::string io2 = io;

		io2.pos = 1;
		io2.outSize = 14;
		io2.write("!12345890$!~@\n");

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
		io2.inSize = 14;

		io2.readStream(str);
		cout << "\nSize: " << str.size() << endl;
		cout << "~~" << str << "~~" << endl << endl;

		io2.read(str);
		cout << "\nSize: " << str.size() << endl;
		cout << "~~" << str << "~~" << endl << endl;
	}
	catch (dodo::exception::basic ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
