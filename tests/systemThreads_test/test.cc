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
	cout << endl << timeTools::now() << endl;
}

int main(int argc, char **argv)
{
	try
	{
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
		
		
		//systemTools::sleep(5);
		
		//th.stopThread(pos);
	}
	catch(baseEx ex)
	{
		cout << ex << endl;
	}
		
	return 0;
}
