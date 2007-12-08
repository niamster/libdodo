#include <libdodo/baseEx.h>
#include <libdodo/systemProcesses.h>
#include <libdodo/systemTools.h>
#include <libdodo/timeTools.h>
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

		cout << endl << (char *)data << ": " << timeTools::now() << endl;
		cout.flush();
		
		systemTools::sleep(10);
		
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
		systemSharedData shD("dodo");
		char *data = (char *)shD.map(100);
		strcpy(data, "abc");

		dgCI = dgC.add((char *)"@test@\n");
		dg.set((char *)"!test!\n");

		systemProcesses pr;

		const int amount = 10;

		int pos[amount];
		std::string ids[amount];
		for (int i=0;i<amount;++i)
		{
			ids[i] = stringTools::lToString(i);
			pos[i] = pr.add(process,(void *)ids[i].c_str());
		}
		
		for (int i=0;i<amount;++i)
			pr.run(pos[i]);
		
		cout << endl << endl << "STARTED" << endl;
		cout << timeTools::now() << endl;
		cout.flush();
		
		pr.wait();
		
		//systemTools::sleep(5);
		
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" <<  ex.file << "\t" << ex.line << endl;
	}
		
	return 0;
}
