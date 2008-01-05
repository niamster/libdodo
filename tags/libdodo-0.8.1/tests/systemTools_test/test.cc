#include <libdodo/baseEx.h>
#include <libdodo/systemTools.h>
#include <libdodo/tools.h>

#include <iostream>

using namespace dodo;
using namespace std;

static bool cought = false;
static bool run = true;
static int number = 1;

void 
signaler(int, siginfo_t *, void *)
{
	cout << "\nTEST\n";
	cout.flush();
	cought = true;
	number++;
}

void 
exit(int, siginfo_t *, void *)
{
	run = false;
}

int main(int argc, char **argv)
{
	
	cout << systemTools::getPID() << endl;
	
	systemTools::setSignalHandler(SYSTEMTOOLS_SIGNAL_HANGUP,exit);
	systemTools::setSignalHandler(SYSTEMTOOLS_SIGNAL_INTERRUPT,signaler);
	
	if (systemTools::isSignalHandled(SYSTEMTOOLS_SIGNAL_HANGUP))
		cout << "SYSTEMTOOLS_SIGNAL_HANGUP IS SET ... !\n";
	else
		cout << "SYSTEMTOOLS_SIGNAL_HANGUP IS NOT SET ... !\n";
		
	while (run)
	{
		cout << "\r" << number;
		if (cought)
		{
			cought = false;
			cout << "\nSYSTEMTOOLS_SIGNAL_HANGUP =)\n";
			cout.flush();
			
			systemTools::unsetSignalHandler(SYSTEMTOOLS_SIGNAL_INTERRUPT);
		}
		
	}
	
	systemTools::setWorkingDir("/");
	
	cout << systemTools::getWorkingDir() << endl;
	
	{
		dodoArray<__userInfo> info;
		systemTools::getUsers(info);
		
		for (int i(0);i<info.size();i++)
			cout << info[i].name << endl;
	}
	
	cout << endl << endl;
	
	{	
		dodoArray<__groupInfo> info;
		systemTools::getGroups(info);
		
		for (int i(0);i<info.size();i++)
			cout << info[i].name << endl;	
	}
		
	cout << systemTools::getWorkingDir() << endl;	

	systemTools::die(stringTools::rTrim("    rTrim    "));

	cout << systemTools::getWorkingDir();
		
	return 0;
}
