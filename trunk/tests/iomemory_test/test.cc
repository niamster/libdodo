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

int main(int argc, char **argv)
{
	try
	{
		io::memory io0;
		io0.block = true;

		io0.writeString("!12345890#!!@\n");
		io0.writeString("!12345890-!!@\n");
		io0.writeString("!12345890@!!@\n");
		io0.writeString("!12345890$!!@\n");

		io::memory io1 = io0;

		io1.pos = 1;
		io1.blockSize = 14;
		io1.write("!12345890$!~@\n");

		/**
		 * io should contain
		 !12345890#!!@
		 !12345890$!~@
		 !12345890@!!@
		 !12345890$!!@
		 */

		dodoString str;

		io1.pos = 2;
		str = io1.readString();
		cout << "\nSize: " << str.size() << endl;
		cout << "~~" << str << "~~" << endl << endl;

		io1.pos = 2;
		str = io1.read();
		cout << "\nSize: " << str.size() << endl;
		cout << "~~" << str << "~~" << endl << endl;

		cout << "io0:" << endl;
		cout << io0 << endl;
		cout << "io1:" << endl;
		cout << io1 << endl;
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
