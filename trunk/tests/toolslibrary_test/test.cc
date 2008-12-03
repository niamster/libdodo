/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>
#include <libdodo/toolsLibrary.h>
#include <libdodo/exceptionBasic.h>

#include <iostream>

using namespace dodo;

using namespace std;

#ifdef DL_EXT
using namespace tools;

typedef void (*print)(const dodoString &data);
#endif

int main(int argc, char **argv)
{

	try
	{

#ifdef DL_EXT

#ifdef BFD_EXT
		dodoStringArray arr = library::getSymbols("./module");
		dodoStringArray::iterator i = arr.begin(), j = arr.end();
		for (; i != j; ++i)
			cout << *i << endl;
		cout << endl;
#endif

		library sll("./module");


		((print)sll["print"])("operator[]");

		((print)sll.get("print"))("get");

#endif

	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << endl;
	}
	return 0;
}
