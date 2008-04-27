#include <libdodo/baseEx.h>
#include <libdodo/systemProcessCollection.h>
#include <libdodo/toolsSystem.h>
#include <libdodo/toolsTime.h>
#include <libdodo/tools.h>
#include <libdodo/systemSharedData.h>
#include <libdodo/systemProcessSharedDataGuard.h>
#include <libdodo/systemProcessSharedDataCollectionGuard.h>

#include <iostream>

using namespace dodo;
using namespace std;

systemProcessSharedDataGuard dg;
systemProcessSharedDataCollectionGuard dgC;
unsigned long dgCI;

void *
process(void *data)
{
	try
	{
		systemSharedData shD("dodo");
		char *data = (char *)shD.map(100);
		cout << data << endl;

		cout << (char *)dgC.get(dgCI);
		cout << (char *)dg.lock();dg.unlock();

		cout << endl << (char *)data << ": " << toolsTime::now() << endl;
		cout.flush();
		
		toolsSystem::sleep(10);
		
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
	try
	{
		systemSharedData shD("dodo");
		char *data = (char *)shD.map(100);
		strcpy(data, "test");

		dgCI = dgC.add((char *)"@test@\n");
		dg.set((char *)"!test!\n");

		systemProcessCollection pr;

		const int amount = 10;

		int pos[amount];
		dodoString ids[amount];
		for (int i=0;i<amount;++i)
		{
			ids[i] = toolsString::lToString(i);
			pos[i] = pr.add(process,(void *)ids[i].c_str());
		}
		
		for (int i=0;i<amount;++i)
			pr.run(pos[i]);
		
		cout << endl << endl << "STARTED" << endl;
		cout << toolsTime::now() << endl;
		cout.flush();
		
		pr.wait();
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}
		
	return 0;
}
