#include <libdodo/baseEx.h>
#include <libdodo/toolsSystem.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsTime.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		cout << toolsTime::byFormat("%A-%Y-%H",toolsTime::now()) << endl;
		
		cout << toolsTime::now() << endl;
		
		toolsSystem::sleep(3);
		
		cout << toolsTime::now() << endl;
		
		cout << toolsTime::week(toolsTime::now(),"%A")[0] << endl;
		
		cout << toolsTime::time(toolsTime::now()).sec << endl;
	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << endl;
	}
		
	return 0;
}
