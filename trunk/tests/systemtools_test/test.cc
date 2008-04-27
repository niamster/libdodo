#include <libdodo/baseEx.h>
#include <libdodo/toolsSystem.h>
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
	
	cout << toolsSystem::getPID() << endl;
	
	toolsSystem::setSignalHandler(SYSTEMTOOLS_SIGNAL_HANGUP,exit);
	toolsSystem::setSignalHandler(SYSTEMTOOLS_SIGNAL_INTERRUPT,signaler);
	
	if (toolsSystem::isSignalHandled(SYSTEMTOOLS_SIGNAL_HANGUP))
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
			
			toolsSystem::unsetSignalHandler(SYSTEMTOOLS_SIGNAL_INTERRUPT);
		}
		
	}
	
	toolsSystem::setWorkingDir("/");
	
	cout << toolsSystem::getWorkingDir() << endl;
	
	{
		dodoArray<__userInfo> info = toolsSystem::getUsers();
		
		for (int i(0);i<info.size();i++)
			cout << info[i].name << endl;
	}
	
	cout << endl << endl;
	
	{	
		dodoArray<__groupInfo> info = toolsSystem::getGroups();
		
		for (int i(0);i<info.size();i++)
			cout << info[i].name << endl;	
	}
		
	cout << toolsSystem::getWorkingDir() << endl;	

	toolsSystem::die(toolsString::rTrim("    rTrim    "));

	cout << toolsSystem::getWorkingDir();
		
	return 0;
}
