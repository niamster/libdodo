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
		cout << filesystem::fileContents("test.cc");
		filesystem::copy("test.cc", "test.cc.copy", true);

		cout << tools::code::MD5Hex(filesystem::fileContents("test.cc")) << endl;
		cout << tools::code::MD5Hex(filesystem::fileContents("test.cc.copy")) << endl;

		dodoStringArray arr = filesystem::fileContentsInArray("test.cc");
		for (unsigned int i = 0; i < arr.size(); i++)
		{
			cout << arr[i];
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}
	catch (...)
	{
		cout << "not exists\n";
	}

	try
	{
		filesystem::rm("./test.cc.copy");

		filesystem::mkdir("dir");
		filesystem::mkdir("dir/1/2/3/4");

		filesystem::rm("_dir_");
		filesystem::rename("dir", "_dir_");

		filesystem::symlink("test", "TEST");

		cout << filesystem::permissions("Makefile") << endl;

		dodoArray<filesystem::__file__> dir = filesystem::dir("./");
		if (dir.size() > 0)
		{
			cout << endl << dir.size() << endl;
			for (dodoArray<filesystem::__file__>::iterator i = dir.begin(); i != dir.end(); ++i)
				cout << i->size << "\t\t-- " << i->name << endl;
		}

		filesystem::unlink("test.dat");

		file::regular io0;
		io0.open("test.dat", file::regular::OPEN_MODE_READ_WRITE_TRUNCATE);
		filesystem::chmod("test.dat", filesystem::PERMISSION_ALL_ALL_ACCESS);

		io0.writeString("!12345890#!!@\n");
		io0.writeString("!12345890-!!@\n");
		io0.writeString("!12345890@!!@\n");
		io0.writeString("!12345890$!!@\n");

		file::regular io1 = io0;
		io0.close();

		io1.pos = 1;
		io1.blockSize = 14;
		io1.block = true;
		io1.write("!12345890$!~@\n");
		/**
		 * test.dat should contain
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

		str = io1.read();
		cout << "\nSize: " << str.size() << endl;
		cout << "~~" << str << "~~" << endl << endl;
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
