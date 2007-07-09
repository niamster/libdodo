#include <baseEx.h>
#include <systemJob.h>
#include <systemProcesses.h>
#include <systemThreads.h>
#include <systemTools.h>
#include <timeTools.h>
#include <tools.h>
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
		cout << ex << ex.line << endl;
	}	
	
	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
		

		const int amount = 10;
		
		systemJob *pr[amount];

		int pos[amount];
		std::string ids[amount];
		for (int i=0;i<amount;++i)
		{
			if (i%2 == 0)	
				pr[i] = new systemProcesses;
			else
				pr[i] = new systemThreads;

			ids[i] = tools::lToString(i);
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
		
		//systemTools::sleep(5);
		
	}
	catch(baseEx ex)
	{
		cout << ex << endl;
	}
		
	return 0;
}
