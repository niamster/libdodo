#include <libdodo/baseEx.h>
#include <libdodo/systemThreads.h>
#include <libdodo/systemThreadSharedDataGuard.h>
#include <libdodo/systemTools.h>
#include <libdodo/timeTools.h>
#include <libdodo/tools.h>

#include <iostream>

using namespace dodo;
using namespace std;

systemThreadSharedDataGuard sh;

void *
thread(void *data)
{
	try
	{
		cout << endl << (char *)data << ": " << timeTools::now() << endl;
		cout.flush();
		
		int timeout = *(int *)sh.lock();
		systemTools::sleep(timeout);
		sh.unlock();
		
		cout << endl << (char *)data << ": " << timeTools::now() << endl;
		cout.flush();
	}
	catch(baseEx ex)
	{
		cout << (string)ex << ex.line << endl;
	}	
	
	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
		int *shared = new int(1);
		
		sh.set((void *)shared);
		
		systemThreads th;

		const int amount = 10;

		int pos[amount];
		std::string ids[amount];
		for (int i=0;i<amount;++i)
		{
			ids[i] = stringTools::lToString(i);
			pos[i] = th.add(thread,(void *)ids[i].c_str());
		}
		
		for (int i=0;i<amount;++i)
			th.run(pos[i]);
		
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
		cout << (string)ex << endl;
	}
		
	return 0;
}
