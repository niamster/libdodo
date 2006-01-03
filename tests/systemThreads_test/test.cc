#include <systemThreads.h>
#include <systemThreadShares.h>
#include <systemTools.h>
#include <timeTools.h>
#include <tools.h>

using namespace dodo;
using namespace std;

void *
thread(void *data)
{
	cout << endl << endl << (char *)data << endl;
	cout << timeTools::now();
	cout.flush();
	long i = -1000000000;
	while (i<1000000000){i++;}
}

int main(int argc, char **argv)
{
	try
	{
		systemThreads th;
		int pos = th.addThread(thread,(void *)"HIHI");
		th.runThread(pos);
		
		cout << endl << endl << "STARTED" << endl;
		cout << timeTools::now();
		cout.flush();
		
		while (true)
		{
			th.waitThread(pos);
			//break;
		}
		
		//systemTools::sleep(5);
		
		//th.stopThread(pos);
	}
	catch(baseEx ex)
	{
		cout << ex << endl;
	}
		
	return 0;
}
