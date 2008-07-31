/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace tools;

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

	cout << os::getPID() << endl;

	os::setSignalHandler(OS_SIGNAL_HANGUP, exit);
	os::setSignalHandler(OS_SIGNAL_INTERRUPT, signaler);

	if (os::isSignalHandled(OS_SIGNAL_HANGUP))
		cout << "OS_SIGNAL_HANGUP IS SET ... !\n";
	else
		cout << "OS_SIGNAL_HANGUP IS NOT SET ... !\n";

	while (run)
	{
		cout << "\r" << number;
		if (cought)
		{
			cought = false;
			cout << "\nOS_SIGNAL_HANGUP =)\n";
			cout.flush();

			os::unsetSignalHandler(OS_SIGNAL_INTERRUPT);
		}

	}

	os::setWorkingDir("/");

	cout << os::getWorkingDir() << endl;

	{
		dodoArray<__userInfo> info = os::getUsers();

		for (int i(0); i < info.size(); i++)
			cout << info[i].name << endl;
	}

	cout << endl << endl;

	{
		dodoArray<__groupInfo> info = os::getGroups();

		for (int i(0); i < info.size(); i++)
			cout << info[i].name << endl;
	}

	cout << os::getWorkingDir() << endl;

	os::die(tools::string::rTrim("    rTrim    "));

	cout << os::getWorkingDir();

	return 0;
}
