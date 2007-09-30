#include <baseEx.h>
#include <systemTools.h>
#include <tools.h>
#include <iostream>

using namespace dodo;
using namespace std;

static bool cought = false;
static bool run = true;
static int number = 1;

void 
signaler(int, siginfo_t *, void *)
{
	cout << "\nHAHA =)\n";
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
	
	systemTools::setSignalHandler(SIGNAL_HANGUP,exit);
	systemTools::setSignalHandler(SIGNAL_INTERRUPT,signaler);
	//systemTools::unsetSignalHandler(SIGNAL_HANGUP);
	
	if (systemTools::isSignalHandled(SIGNAL_HANGUP))
		cout << "SET ... !\n";
	else
		cout << "NOT SET ... !\n";
		
	while (run)
	{
		cout << "\r" << number;
		if (cought)
		{
			cought = false;
			cout << "\nHEHE =)\n";
			cout.flush();
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

	systemTools::die(stringTools::rTrim("    dsgdfhhdf    "));

	cout << systemTools::getWorkingDir();
		
	return 0;
}
