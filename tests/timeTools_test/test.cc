#include <systemTools.h>
#include <tools.h>
#include <timeTools.h>

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
		
		cout << timeTools::makeTime(timeTools::now()).sec << endl;
	}
	catch (baseEx ex)
	{
		cout << ex << "\t" << ex.line << endl;
	}
		
	return 0;
}
