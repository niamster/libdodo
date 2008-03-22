#include <libdodo/baseEx.h>
#include <libdodo/ioDisk.h>
#include <libdodo/ioDiskTools.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{	
		cout << ioDiskTools::getFileContents("test.cc");
		ioDiskTools::copy("test.cc", "test.cc.copy", true);

		dodoStringArray arr = ioDiskTools::getFileContentsArr("test.cc");		
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
		ioDiskTools::rm("./test.cc.copy");

		ioDiskTools::mkdir("testDir");
		ioDiskTools::mkdirRecursive("testDir/1/2/3/4");

		ioDiskTools::rm("DirTest");
		ioDiskTools::rename("testDir","DirTest");
		
		ioDiskTools::symlink("test", "TEST");
		ioDiskTools::chmod("my.dat", IODISKTOOLS_PERM_ALL_ALL_ACCESS);
		
		cout << ioDiskTools::getPermissions("Makefile") << endl;

		dodoArray<__fileInfo> dir = ioDiskTools::getDirInfo("./");
		if (dir.size() > 0)
		{
			cout << endl << dir.size() << endl;
			for (dodoArray<__fileInfo>::iterator i=dir.begin();i!=dir.end();++i)
				cout << i->size << "!" << i->name << endl; 
		}
		
		ioDiskTools::unlink("my.dat");
		
		ioDisk io("my.dat");
		
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