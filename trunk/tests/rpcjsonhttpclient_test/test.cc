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

class httpIO : public io::channel, public io::network::http
{
  public:

	httpIO() : io::channel(io::CHANNEL_PROTECTION_NONE),
			   response(NULL)
	{
	}

  protected:

	virtual void _read(char * const data)
	{
		_readStream(data);
	}

	virtual unsigned long _readStream(char * const data)
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

	virtual void _write(const char * const data)
	{
		if (response != NULL)
		{
			delete response;
			response = NULL;
		}

		dodoString post_data(data, outSize);

		response = new io::network::__httpResponse__(POST(post_data, "application/json"));
	}

	virtual void _writeStream(const char * const data)
	{
		if (response != NULL)
		{
			delete response;
			response = NULL;
		}

		unsigned int dataLen = strlen(data);

		dodoString post_data(data, dataLen>outSize?outSize:dataLen);

		response = new io::network::__httpResponse__(POST(post_data, "application/json"));
	}

	int getOutDescriptor() const
	{
		return -1;
	}

	int getInDescriptor() const
	{
		return -1;
	}

	void flush()
	{
	}

	io::network::__httpResponse__ *response;
};

int main(int argc, char **argv)
{
	try
	{
		httpIO http;
		json::client client(http);

		http.setUrl("http://localhost/libdodo/rpcjsoncgiserver_test/test.cgi");

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

		resp = client.call(method);

		cout << "Amount of values: " << resp.getValues().size() << endl;
		cout << "Response ID: " << client.getResponseId() << endl;
		cout << "First value: " << resp.getValue().getString() << endl;
		cout << "Second value: " << resp.getValue(1).getString() << endl;
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
