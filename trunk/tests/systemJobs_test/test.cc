#include <libdodo/baseEx.h>
#include <libdodo/systemJobs.h>
#include <libdodo/systemProcesses.h>
#include <libdodo/systemThreadCollection.h>
#include <libdodo/systemTools.h>
#include <libdodo/timeTools.h>
#include <libdodo/tools.h>

#include <iostream>

using namespace dodo;
using namespace std;

void *
process(void *data)
{
	try
	{
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
		

		const int amount = 10;
		
		systemJobs *pr[amount];

		int pos[amount];
		std::string ids[amount];
		for (int i=0;i<amount;++i)
		{
			if (i%2 == 0)	
				pr[i] = new systemProcesses;
#ifdef PTHREAD_EXT
			else
				pr[i] = new systemThreadCollection;
#endif

			ids[i] = stringTools::lToString(i);
			pos[i] = pr[i]->add(process,(void *)ids[i].c_str());
		}
		
		for (int i=0;i<amount;++i)
			pr[i]->run(pos[i]);
		
		cout << endl << endl << "STARTED" << endl;
		cout << timeTools::now() << endl;
		cout.flush();
		
		for (int i=0;i<amount;++i)
			pr[i]->wait();

		for (int i=0;i<amount;++i)
			delete pr[i];
	}
	catch(baseEx ex)
	{
		cout << (string)ex << endl;
	}
		
	return 0;
}
