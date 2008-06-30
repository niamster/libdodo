/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/baseEx.h>
#include <libdodo/ioFile.h>
#include <libdodo/toolsFilesystem.h>

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

		cout << tools::misc::MD5Hex(filesystem::getFileContents("test.cc")) << endl;
		cout << tools::misc::MD5Hex(filesystem::getFileContents("test.cc.copy")) << endl;

		dodoStringArray arr = filesystem::getFileContentsArr("test.cc");
		for (int i = 0; i < arr.size(); i++)
		{
			cout << arr[i];
		}
	}
	catch (baseEx ex)
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
		filesystem::mkdirRecursive("testDir/1/2/3/4");

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

		filesystem::unlink("my.dat");

		file io("my.dat", FILE_FILETYPE_REG_FILE, FILE_OPENMODE_READ_WRITE_TRUNCATE);
		filesystem::chmod("my.dat", FILESYSTEM_PERMISSION_ALL_ALL_ACCESS);

		io.inSize = io.outSize = 14;
		io.over = true;

		io.writeStreamString("!12345890#!!@\n");
		io.writeStreamString("!12345890-!!@\n");
		io.writeStreamString("!12345890@!!@\n");
		io.writeStreamString("!12345890$!!@\n");
		io.pos = 1;
		io.writeString("!12345890$!~@\n");
		/**
		 * my.dat should contain
		 !12345890#!!@
		 !12345890$!~@
		 !12345890@!!@
		 !12345890$!!@
		 */

		dodoString str;

		io.pos = 0;
		io.readStreamString(str);
		cout << "\n\n" << str << "\n\n";

		io.pos = 0;
		io.readString(str);
		cout << "\n\n" << str << "\n\n";
	}
	catch (baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << "\t" << ex.file << endl;
	}
	return 0;
}
