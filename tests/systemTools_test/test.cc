#include <systemTools.h>
#include <tools.h>

using namespace dodo;



using namespace std;

int main(int argc, char **argv)
{
	systemTools::setWorkingDir("/");
	
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

	cout << systemTools::getWorkingDir();	
		
	return 0;
}
