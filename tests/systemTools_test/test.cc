#include <systemTools.h>
#include <tools.h>

using namespace dodo;
using namespace std;

static bool cought = false;

void 
signaler(int, siginfo_t *, void *)
{
	cout << "HAHA =)";
	cout.flush();
	cought = true;
}

int main(int argc, char **argv)
{
	
	systemTools::setSignalHandler(SIGNAL_KILL,signaler);
/*	if (systemTools::isSignalHandled(SIGNAL_HANGUP))
		cout << "SET ... !\n";
	else
		cout << "NOT SET ... !\n";*/
		
	while (true)
	{
		//if (cought)
		{
			cought = false;
			cout << "HEHE =)\n";
			cout.flush();
		}
		
	}
	
/*	systemTools::setWorkingDir("/");
	
	cout << systemTools::getWorkingDir() << endl;
	
	{
		std::vector<__userInfo> info;
		systemTools::getUsers(info);
		
		for (int i(0);i<info.size();i++)
			cout << info[i].name << endl;
	}
	
	cout << endl << endl;
	
	{	
		std::vector<__groupInfo> info;
		systemTools::getGroups(info);
		
		for (int i(0);i<info.size();i++)
			cout << info[i].name << endl;	
	}
		
	cout << systemTools::getWorkingDir() << endl;	

	systemTools::die(tools::rTrim("    dsgdfhhdf    "));

	cout << systemTools::getWorkingDir();*/
		
	return 0;
}
