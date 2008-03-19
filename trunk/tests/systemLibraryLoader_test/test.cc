#include <libdodo/systemLibraryLoader.h>
#include <libdodo/directives.h>

#include <iostream>

using namespace dodo;
using namespace std;

#ifdef DL_EXT
	typedef void (* print)(const std::string &data);
#endif

int main(int argc, char **argv)
{

	try
	{

#ifdef DL_EXT
		systemLibraryLoader sll("./module");

		print pr;

		pr = (print)sll["print"];
		pr("operator");

		pr = (print)sll.get("print");
		pr("get");
		
#endif
	
	}
	catch(baseEx ex)
	{
		cout << (string)ex << "\t" << ex.line << endl;
	}
	return 0;
}
