#include <flushDisk.h>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{

	try
	{	
		flushDisk::unlink("my.dat");
	}
	catch(...)
	{
		cout << "not exists\n";
	}
	
	//flushDisk flush(FIFO_FILE,"my.dat");
	flushDisk flush(REG_FILE,"my.dat");
	//flushDisk flush(true);///in tmp file
	flush.open();
	flush.size = 10;
	
	flush.writeString("!1234567890-!!",0);
	
	string str;

	flush.readString(str,0);
	cout << "\n\n" << str << "\n\n";


//	flushDisk::rm("asas");
	
	flushDisk::mkdir("asas");//,OWNER_ALL_ACCESS,false);
	flushDisk::symlink("test","A");
	flushDisk::chmod("test",ALL_ALL_ACCESS);
	
	cout << flushDisk::getPermissions("Makefile");
	
	std::vector<__fileInfo> dir;
	
	cout << endl; 
	if (flushDisk::getDirInfo("/",dir))
	{
		cout << endl << dir.size() << endl;
		for (std::vector<__fileInfo>::iterator i=dir.begin();i!=dir.end();++i)
			cout << i->size << "!" << i->name << endl; 
	}
	
	return 0;
}
