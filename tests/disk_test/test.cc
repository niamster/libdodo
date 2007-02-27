#include <baseEx.h>
#include <flushDisk.h>
#include <flushDiskTools.h>
#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{	
		//cout << flushDiskTools::getFileContent("test.cc");
		//flushDiskTools::copy("test.cc","test.cc.copy",true);
		//flushDiskTools::copy("/tmp","./",true);
		/*dodoStringArr arr = flushDiskTools::getFileContentArr("test.cc");
		
		for (int i=0;i<arr.size();i++)
		{
			cout << arr[i];
		}*/
		//flushDiskTools::copy("/root","./tmp",true);
		//flushDiskTools::copy("/root","./tmp/",true);
		//flushDiskTools::unlink("my.dat");
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
		flushDiskTools::unlink("my.dat");
		
		//flushDisk flush(FILETYPE_FIFO_FILE,"my.dat");
		flushDisk flush(FILETYPE_REG_FILE,"my.dat");
		
		
		//flushDisk flush(true);///in tmp file
		flush.open();
		//flush.inSize = flush.outSize = 13;
		flush.over = true;
		
//		flush.append = true;
		
		flush.writeStreamString("!1234567890#!!");
//		flush.writeStreamString("!1234567890-!!");
//		flush.writeStreamString("!1234567890@!!");
		flush.writeStreamString("!1234567890$!!");


		
		string str;
	
		flush.readStreamString(str,0);
		cout << "\n\n" << str << "\n\n";

		flush.readString(str,0);
		cout << "\n\n" << str << "\n\n";

	
		flushDiskTools::rm("./tmp");

		flushDiskTools::mkdir("testDir");//,PERM_OWNER_ALL_ACCESS,false);

		flushDiskTools::rename("testDir","DirTest");	
		
		flushDiskTools::symlink("test","TEST");
		flushDiskTools::chmod("test",PERM_ALL_ALL_ACCESS);
		
		cout << flushDiskTools::getPermissions("Makefile");
		
		cout << endl; 


		dodoArray<__fileInfo> dir = flushDiskTools::getDirInfo("/");
		
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
