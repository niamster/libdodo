#include <libdodo/baseEx.h>
#include <libdodo/systemTools.h>
#include <libdodo/tools.h>
#include <libdodo/timeTools.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		cout << timeTools::byFormat("%A-%Y-%H",timeTools::now()) << endl;
		
		cout << timeTools::now() << endl;
		
		systemTools::sleep(3);
		
		cout << timeTools::now() << endl;
		
		cout << timeTools::week(timeTools::now(),"%A")[0] << endl;
		
		cout << timeTools::time(timeTools::now()).sec << endl;
	}
	catch (baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << endl;
	}
		
	return 0;
}
