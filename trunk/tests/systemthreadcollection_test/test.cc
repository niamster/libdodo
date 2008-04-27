#include <libdodo/baseEx.h>
#include <libdodo/systemThreadCollection.h>
#include <libdodo/systemThreadSharedDataGuard.h>
#include <libdodo/systemThreadSharedDataCollectionGuard.h>
#include <libdodo/toolsSystem.h>
#include <libdodo/toolsTime.h>
#include <libdodo/tools.h>

#include <iostream>

using namespace dodo;
using namespace std;

systemThreadSharedDataGuard sh;
systemThreadSharedDataCollectionGuard shC;
unsigned long shCI;

void *
thread(void *data)
{
	try
	{
		cout << (char *)shC.get(shCI);
		cout << endl << (char *)data << ": " << toolsTime::now() << endl;
		cout.flush();
		
		int timeout = *(int *)sh.lock();
		toolsSystem::sleep(timeout);
		sh.unlock();
		
		cout << endl << (char *)data << ": " << toolsTime::now() << endl;
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
	shCI = shC.add((char *)"@test@\n");
	try
	{
		int *shared = new int(1);
		
		sh.set((void *)shared);

#ifdef PTHREAD_EXT
		systemThreadCollection th;

		const int amount = 10;

		int pos[amount];
		dodoString ids[amount];
		for (int i=0;i<amount;++i)
		{
			ids[i] = toolsString::lToString(i);
			pos[i] = th.add(thread,(void *)ids[i].c_str());
		}
		
		for (int i=0;i<amount;++i)
			th.run(pos[i]);
		
		cout << endl << endl << "STARTED" << endl;
		cout << toolsTime::now() << endl;
		cout.flush();
		
		th.wait();
#endif
		
		delete shared;
	}
	catch(baseEx ex)
	{
		cout << (string)ex << endl;
	}
		
	return 0;
}
