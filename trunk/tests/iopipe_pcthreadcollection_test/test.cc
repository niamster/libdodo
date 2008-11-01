/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/pcThreadCollection.h>
#include <libdodo/ioPipe.h>
#include <libdodo/toolsTime.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace dodo::pc::thread;

using namespace std;

void *
threadRead(void *data)
{
	cout << "READER->\n";
	cout.flush();

	try
	{
		dodoString str;

		io::pipe *pipe = (io::pipe *)data;

		pipe->readStreamString(str);
		cout << "%" << str << "%\n";
		cout.flush();

		pipe->readStreamString(str);
		cout << "%" << str << "%\n";
		cout.flush();

		pipe->inSize = tools::string::stringToUL(str);
		pipe->readString(str);
		cout << "%" << str << "%\n";
		cout.flush();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line << endl;
	}
	
	cout << "<-READER\n";
	cout.flush();

	return NULL;
}

void *
threadWrite(void *data)
{
	cout << "WRITER->\n";
	cout.flush();

	try
	{
		dodoString str;

		io::pipe *pipe = (io::pipe *)data;

		pipe->writeStreamString(tools::time::byFormat("%H:%M:%S", tools::time::now()));
		pipe->writeStreamString("\n");
		pipe->flush();
		
		str = tools::filesystem::getFileContents("test.cc");
		
		pipe->writeStreamString(tools::string::ulToString(str.size()));
		pipe->writeStreamString("\n");
		pipe->flush();

		pipe->outSize = str.size();
		pipe->writeString(str);
		pipe->flush();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line << endl;
	}
	
	cout << "<-WRITER\n";
	cout.flush();

	return NULL;
}

int main(int argc, char **argv)
{
	try
	{
#ifdef PTHREAD_EXT

		collection j;

		io::pipe pipe1;
		pipe1.open();

		///< write first to avoid deadlock due to io::pipe is threadsafe and here one object is used
		j.addNRun(&threadWrite, (void *)&pipe1);
		tools::os::sleep(1);
		j.addNRun(&threadRead, (void *)&pipe1);

		j.wait();

		io::pipe pipe2 = pipe1;
		//or clone
		//pipe2.clone(pipe1);

		///< use a copy, so no need to keep an order
		j.addNRun(&threadRead, (void *)&pipe1);
		j.addNRun(&threadWrite, (void *)&pipe2);
		
		j.wait();

#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line << endl;
	}

	return 0;
}
