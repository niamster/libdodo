#include <flushDisk.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{

	try
	{	
		flushDisk::unlink("my.dat");
	}
	catch(baseEx ex)
	{
		cout << ex.baseErrstr << endl;
	}
	catch(...)
	{
		cout << "not exists\n";
	}
	
	try
	{
		//flushDisk flush(FIFO_FILE,"my.dat");
		flushDisk flush(REG_FILE,"my.dat");
		
		std::cout << typeid(flush).name();
		
		//flushDisk flush(true);///in tmp file
		flush.open();
	//	flush.autoOutSize = false;flush.inSize = flush.outSize = 10;
		
		flush.writeString("!1234567890-!!",0);
		
		string str;
	
		flush.readString(str,0);
		cout << "\n\n" << str << "\n\n";
	
	
		flushDisk::rm("asas");
		flushDisk::rm("log");

		flushDisk::mkdir("asas");//,OWNER_ALL_ACCESS,false);

		flushDisk::rename("asas","hiho");	
		
		flushDisk::symlink("test","A");
		flushDisk::chmod("test",ALL_ALL_ACCESS);
		
		cout << flushDisk::getPermissions("Makefile");
		
		cout << endl; 
		std::vector<__fileInfo> dir = flushDisk::getDirInfo("/");
		
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
