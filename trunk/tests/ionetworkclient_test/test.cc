/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/exceptionBasic.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/types.h>

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
			client st(CONNECTION_PROTO_FAMILY_IPV4, CONNECTION_TRANSFER_TYPE_STREAM);
			exchange exch;
			dodoString str;

			try
			{
				st.connectFrom("127.0.0.1", "127.0.0.1", 21, exch);

				exch.readStreamString(str);
				cout << "\ngot:\n" << str << "\n";
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
