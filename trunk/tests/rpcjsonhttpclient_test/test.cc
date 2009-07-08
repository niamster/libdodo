/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>
#include <string.h>

using namespace dodo;
using namespace rpc;

using namespace std;

class httpIO : public io::stream::channel, public io::network::http::client
{
  public:

	httpIO(const dodoString &url) : io::stream::channel(io::CHANNEL_PROTECTION_NONE),
									response(NULL),
									io::network::http::client(url)
	{
	}

	~httpIO()
	{
		if (response != NULL)
			delete response;
	}

  protected:

	virtual void _read(char * const data) const
	{
		_readStream(data);
	}

	virtual unsigned long _readStream(char * const data) const
	{
		unsigned long size = 0;

		if (response != NULL)
		{
			size = response->data.size();
			if (size != 0)
			{
				if (size > inSize)
					size = inSize;

				memcpy(data, response->data.data(), size);
				if (size < inSize)
					memset(data+size, 0x0, inSize - size);
			}

			delete response;
			response = NULL;
		}

		return size;
	}

	virtual void _write(const char * const data) const
	{
		throw dodoString("Not implemented");
	}

	virtual void _writeStream(const char * const idata) const
	{
		data.append(idata);
	}

	int getOutDescriptor() const
	{
		return -1;
	}

	int getInDescriptor() const
	{
		return -1;
	}

	void flush() const
	{
		if (response != NULL)
		{
			delete response;
			response = NULL;
		}

		response = new io::network::http::response(POST(data, "application/json"));
		data.clear();
	}

	mutable io::network::http::response *response;
	mutable dodoString data;
};

int main(int argc, char **argv)
{
	try
	{
		json::client client(httpIO("http://localhost/libdodo/rpcjsoncgiserver_test/test.cgi"));

		method method;
		value argument;

		method.setName("callTest");

		argument.setString("argument");
		method.addArgument(argument);

		argument.setBoolean(true);
		method.addArgument(argument);

		argument.addStructMember("string", dodoString("string"));
		argument.addStructMember("integer", (long)10);
		method.addArgument(argument);

		argument.addArrayElement(dodoString("string"));
		argument.addArrayElement((double)10.0);
		method.addArgument(argument);

		response resp = client.call(method);

		cout << "Amount of values: " << resp.getValues().size() << endl;
		cout << "Response ID: " << client.getResponseId() << endl;
		cout << "First value: " << resp.getValue().getString() << endl;
		cout << "Second value: " << resp.getValue(1).getString() << endl;

		/* resp = client.call(method); */

		/* cout << "Amount of values: " << resp.getValues().size() << endl; */
		/* cout << "Response ID: " << client.getResponseId() << endl; */
		/* cout << "First value: " << resp.getValue().getString() << endl; */
		/* cout << "Second value: " << resp.getValue(1).getString() << endl; */
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << endl;
	}

	return 0;
}
