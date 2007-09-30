#include <baseEx.h>
#include <systemProcesses.h>
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
		cout << (string)ex << ex.line << endl;
	}	
	
	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
		
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
		cout << (string)ex << endl;
	}
		
	return 0;
}
