#include <systemThreads.h>
#include <systemThreadShares.h>
#include <systemTools.h>
#include <timeTools.h>
#include <tools.h>

using namespace dodo;
using namespace std;

systemThreadShares sh;
int shPos = 0;

void *
thread(void *data)
{
	try
	{
		cout << endl << endl << (char *)data << timeTools::now() << endl;
		cout.flush();
		
		int *timeout = (int *)sh.lock(shPos,100000);
		cout << "==" << *timeout << endl;
		cout.flush();
		systemTools::sleep(*timeout);
		sh.unlock(shPos);
		
		cout << endl << endl << (char *)data << timeTools::now() << endl;
		cout.flush();
	}
	catch(baseEx ex)
	{
		cout << ex << ex.line << endl;
	}	
	//cout.flush();
	//long i = -1000000000;
	//while (i<1000000000){i++;}
	//cout << endl << timeTools::now() << endl;
}

int main(int argc, char **argv)
{
	try
	{
		int *shared = new int(10);
		
		shPos = sh.add((void *)shared);
		
		systemThreads th;
		int pos[2];
		pos[0] = th.add(thread,(void *)"HIHI");
		pos[1] = th.add(thread,(void *)"HOHO");
		
		th.run(pos[0]);
		th.run(pos[1]);
		
		//thread((void *)"HIHI");
		//thread((void *)"HOHO");
		
		cout << endl << endl << "STARTED" << endl;
		cout << timeTools::now() << endl;
		cout.flush();
		
		th.wait();
		
		//systemTools::sleep(5);
		
		//th.stopThread(pos);
		
		delete shared;
	}
	catch(baseEx ex)
	{
		cout << ex << endl;
	}
		
	return 0;
}
