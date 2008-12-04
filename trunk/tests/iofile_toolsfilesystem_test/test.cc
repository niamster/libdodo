/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/ioFile.h>
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
		cout << filesystem::getFileContents("test.cc");
		filesystem::copy("test.cc", "test.cc.copy", true);

		cout << tools::code::MD5Hex(filesystem::getFileContents("test.cc")) << endl;
		cout << tools::code::MD5Hex(filesystem::getFileContents("test.cc.copy")) << endl;

		dodoStringArray arr = filesystem::getFileContentsArr("test.cc");
		for (int i = 0; i < arr.size(); i++)
		{
			cout << arr[i];
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << endl;
	}
	catch (...)
	{
		cout << "not exists\n";
	}

	try
	{
		filesystem::rm("./test.cc.copy");

		filesystem::mkdir("testDir");
		filesystem::mkdir("testDir/1/2/3/4");

		filesystem::rm("DirTest");
		filesystem::rename("testDir", "DirTest");

		filesystem::symlink("test", "TEST");

		cout << filesystem::getPermissions("Makefile") << endl;

		dodoArray<__fileInfo> dir = filesystem::getDirInfo("./");
		if (dir.size() > 0)
		{
			cout << endl << dir.size() << endl;
			for (dodoArray<__fileInfo>::iterator i = dir.begin(); i != dir.end(); ++i)
				cout << i->size << "!" << i->name << endl;
		}

		filesystem::unlink("test.dat");

		file::regular io;
		io.open("test.dat", file::REGULAR_OPENMODE_READ_WRITE_TRUNCATE);
		filesystem::chmod("test.dat", FILESYSTEM_PERMISSION_ALL_ALL_ACCESS);

		io.overwrite = true;

		io.writeStream("!12345890#!!@\n");
		io.writeStream("!12345890-!!@\n");
		io.writeStream("!12345890@!!@\n");
		io.writeStream("!12345890$!!@\n");

		file::regular io2 = io;
		io.close();

		io2.pos = 1;
		io2.outSize = 14;
		io2.write("!12345890$!~@\n");
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

		str = io2.readStream();
		cout << "\nSize: " << str.size() << endl;
		cout << "~~" << str << "~~" << endl << endl;

		str = io2.read();
		cout << "\nSize: " << str.size() << endl;
		cout << "~~" << str << "~~" << endl << endl;
	}
	catch (dodo::exception::basic ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
