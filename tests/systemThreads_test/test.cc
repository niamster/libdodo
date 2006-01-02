#include <systemThreads.h>
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
		
		th.waitThread(pos);
		
		//systemTools::sleep(5);
	}
	catch(baseEx ex)
	{
		cout << ex << endl;
	}
		
	return 0;
}
