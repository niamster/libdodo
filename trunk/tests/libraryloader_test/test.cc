/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>
#include <libdodo/libraryLoader.h>
#include <libdodo/exceptionBasic.h>

#include <iostream>

using namespace dodo;

using namespace std;

#ifdef DL_EXT
typedef void (*print)(const dodoString &data);
#endif

int main(int argc, char **argv)
{

	try
	{

#ifdef DL_EXT

#ifdef BFD_EXT
		dodoStringArray arr = libraryLoader::getSymbols("./module");
		dodoStringArray::iterator i = arr.begin(), j = arr.end();
		for (; i != j; ++i)
			cout << *i << endl;
		cout << endl;
#endif

		libraryLoader sll("./module");

		print pr;

		pr = (print)sll["print"];
		pr("operator");

		pr = (print)sll.get("print");
		pr("get");

#endif

	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << "\t" << ex.line << endl;
	}
	return 0;
}
