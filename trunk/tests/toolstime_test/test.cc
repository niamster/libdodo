/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsTime.h>

#include <iostream>

using namespace dodo;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		cout << tools::time::byFormat("%A-%Y-%H", tools::time::now()) << endl;

		cout << tools::time::now() << endl;

		tools::os::sleep(3);

		cout << tools::time::now() << endl;

		cout << tools::time::week(tools::time::now(), "%A")[0] << endl;

		cout << tools::time::timestamp(tools::time::now()).sec << endl;
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << endl;
	}

	return 0;
}
