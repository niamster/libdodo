/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace tools;

using namespace std;

static bool cought = false;
static bool run = true;
static int number = 1;

void
handler(int signal, void *, void *)
{
	cout << endl << "Signal: " << signal << endl;
	cout.flush();
	cought = true;
	number++;
}

void
exit(int, void *, void *)
{
	run = false;
}

int main(int argc, char **argv)
{

	cout << "PID: " << os::PID() << endl;

	os::setSignalHandler(os::SIGNAL_HANGUP, ::exit);
	os::setSignalHandler(os::SIGNAL_INTERRUPT, ::handler);

	if (os::isSignalHandled(os::SIGNAL_HANGUP))
		cout << "SIGNAL_HANGUP is set" << endl;
	else
		cout << "SIGNAL_HANGUP is not set" << endl;
	cout << "Send SIGNAL_HANGUP to stop the loop" << endl;

	while (run)
	{
		if (cought)
		{
			cought = false;
			cout << endl << "SIGNAL_INTERRUPT" << endl;
			cout.flush();

			os::removeSignalHandler(os::SIGNAL_INTERRUPT);

			cout << endl << "Handler for SIGNAL_INTERRUPT unregistered" << endl;
			cout.flush();
		}

	}

	os::setWorkingDir("/");

	cout << os::workingDir() << endl;

	{
		dodoArray<os::__user__> info = os::users();

		for (unsigned int i(0); i < info.size(); i++)
			cout << info[i].name << endl;
	}

	cout << endl << endl;

	{
		dodoArray<os::__group__> info = os::groups();

		for (unsigned int i(0); i < info.size(); i++)
			cout << info[i].name << endl;
	}

	cout << os::workingDir() << endl;

	os::die("DIE message\n");

	cout << os::workingDir();

	return 0;
}
