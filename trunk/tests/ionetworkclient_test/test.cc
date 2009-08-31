/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace io::network;

using namespace std;

int main(int argc, char **argv)
{
	try
	{
		try
		{
			client c(connection::PROTOCOL_FAMILY_IPV4, connection::TRANSFER_STREAM);
			exchange ex;
			dodoString str;

			try
			{
				c.connectFrom("127.0.0.1", "127.0.0.1", 21, ex);

				str = ex.readString();
				cout << "\n" << str << "\n";
			}
			catch (dodo::exception::basic ex)
			{
				cout << (dodoString)ex << "\t" << ex.line << endl;
			}
		}
		catch (dodo::exception::basic ex)
		{
			cout << (dodoString)ex << "\t" << ex.line << endl;
		}
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
