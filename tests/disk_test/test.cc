#include <flushDisk.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{	
		//cout << flushDiskTools::getFileContent("test.cc");
		//flushDiskTools::copy("test.cc","test.cc.copy",true);
		//flushDiskTools::copy("/tmp","./",true);
		/*stringArr arr = flushDiskTools::getFileContentArr("test.cc");
		
		for (int i=0;i<arr.size();i++)
		{
			cout << arr[i];
		}*/
		flushDiskTools::copy("/root","./tmp",true);
		flushDiskTools::copy("/root","./tmp/",true);
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
		//flushDisk flush(FIFO_FILE,"my.dat");
		flushDisk flush(REG_FILE,"my.dat");
		
		
		//flushDisk flush(true);///in tmp file
		flush.open();
		//flush.autoOutSize = false;flush.inSize = flush.outSize = 10;
		flush.over = true;
		flush.writeString("!1234567890-!!",0);
		
		string str;
	
		flush.readString(str,0);
		cout << "\n\n" << str << "\n\n";
	
		flushDiskTools::rm("./tmp");

		flushDiskTools::mkdir("testDir");//,OWNER_ALL_ACCESS,false);

		flushDiskTools::rename("testDir","DirTest");	
		
		flushDiskTools::symlink("test","TEST");
		flushDiskTools::chmod("test",ALL_ALL_ACCESS);
		
		cout << flushDiskTools::getPermissions("Makefile");
		
		cout << endl; 


		std::vector<__fileInfo> dir = flushDiskTools::getDirInfo("/");
		
		if (dir.size() > 0)
		{
			cout << endl << dir.size() << endl;
			for (std::vector<__fileInfo>::iterator i=dir.begin();i!=dir.end();++i)
				cout << i->size << "!" << i->name << endl; 
		}
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << "\t" << ex.line << endl;
	}
	return 0;
}
