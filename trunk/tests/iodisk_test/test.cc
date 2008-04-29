#include <libdodo/baseEx.h>
#include <libdodo/ioFile.h>
#include <libdodo/toolsFilesystem.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{	
		cout << toolsFilesystem::getFileContents("test.cc");
		toolsFilesystem::copy("test.cc", "test.cc.copy", true);

		dodoStringArray arr = toolsFilesystem::getFileContentsArr("test.cc");		
		for (int i=0;i<arr.size();i++)
		{
			cout << arr[i];
		}
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << endl;
	}
	catch(...)
	{
		cout << "not exists\n";
	}
	try
	{	
		toolsFilesystem::rm("./test.cc.copy");

		toolsFilesystem::mkdir("testDir");
		toolsFilesystem::mkdirRecursive("testDir/1/2/3/4");

		toolsFilesystem::rm("DirTest");
		toolsFilesystem::rename("testDir","DirTest");
		
		toolsFilesystem::symlink("test", "TEST");
		
		cout << toolsFilesystem::getPermissions("Makefile") << endl;

		dodoArray<__fileInfo> dir = toolsFilesystem::getDirInfo("./");
		if (dir.size() > 0)
		{
			cout << endl << dir.size() << endl;
			for (dodoArray<__fileInfo>::iterator i=dir.begin();i!=dir.end();++i)
				cout << i->size << "!" << i->name << endl; 
		}
		
		toolsFilesystem::unlink("my.dat");
		
		ioFile io("my.dat", IOFILE_FILETYPE_REG_FILE, IOFILE_OPENMODE_READ_WRITE_TRUNCATE);
		toolsFilesystem::chmod("my.dat", TOOLSFILESYSTEM_PERM_ALL_ALL_ACCESS);
		
		io.inSize = io.outSize = 14;
		io.over = true;
		
		io.writeStreamString("!12345890#!!@\n");
		io.writeStreamString("!12345890-!!@\n");
		io.writeStreamString("!12345890@!!@\n");
		io.writeStreamString("!12345890$!!@\n");
		io.pos = 1;
        io.writeString("!12345890$!~@\n");
		
		dodoString str;
	
		io.pos = 0;
		io.readStreamString(str);
		cout << "\n\n" << str << "\n\n";

		io.pos = 0;
		io.readString(str);
		cout << "\n\n" << str << "\n\n";		
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << "\t" << ex.file << endl;
	}
	return 0;
}
