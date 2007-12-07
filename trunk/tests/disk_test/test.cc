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
		//cout << ioDiskTools::getFileContent("test.cc");
		//ioDiskTools::copy("test.cc","test.cc.copy",true);
		//ioDiskTools::copy("/tmp","./",true);
		/*dodoStringArr arr = ioDiskTools::getFileContentArr("test.cc");
		
		for (int i=0;i<arr.size();i++)
		{
			cout << arr[i];
		}*/
		//ioDiskTools::copy("/root","./tmp",true);
		//ioDiskTools::copy("/root","./tmp/",true);
		//ioDiskTools::unlink("my.dat");
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
		ioDiskTools::unlink("my.dat");
		
		//ioDisk io("my.dat", IODISKTOOLS_FILETYPE_FIFO_FILE);
		ioDisk io("my.dat");
		
		//io.inSize = io.outSize = 13;
		io.over = true;
		
//		io.append = true;
		
		io.writeStreamString("!1234567890#!!");
//		io.writeStreamString("!1234567890-!!");
//		io.writeStreamString("!1234567890@!!");
		io.writeStreamString("!1234567890$!!");


		
		dodoString str;
	
		io.readStreamString(str,0);
		cout << "\n\n" << str << "\n\n";

		io.readString(str,0);
		cout << "\n\n" << str << "\n\n";

	
		ioDiskTools::rm("./tmp");

		ioDiskTools::mkdir("testDir");//,IODISKTOOLS_PERM_OWNER_ALL_ACCESS,false);

		ioDiskTools::rename("testDir","DirTest");	
		
		ioDiskTools::symlink("test","TEST");
		ioDiskTools::chmod("test",IODISKTOOLS_PERM_ALL_ALL_ACCESS);
		
		cout << ioDiskTools::getPermissions("Makefile");
		
		cout << endl; 


		dodoArray<__fileInfo> dir = ioDiskTools::getDirInfo("/");
		
		if (dir.size() > 0)
		{
			cout << endl << dir.size() << endl;
			for (dodoArray<__fileInfo>::iterator i=dir.begin();i!=dir.end();++i)
				cout << i->size << "!" << i->name << endl; 
		}
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << endl;
	}
	return 0;
}
