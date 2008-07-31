/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/ipcThreadCollection.h>
#include <libdodo/ioFifo.h>
#include <libdodo/toolsTime.h>
#include <libdodo/toolsOs.h>
#include <libdodo/toolsMisc.h>

#include <iostream>

using namespace dodo;
using namespace dodo::ipc::thread;

using namespace std;

void *
threadRead(void *data)
{
	cout << "READER->\n";
	cout.flush();

	try
	{
		dodoString str;

		io::fifo *fifo = (io::fifo *)data;
		fifo->readStreamString(str);
		
		cout << str;
		cout.flush();
	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << ex.line << endl;
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

		io::fifo *fifo = (io::fifo *)data;
		fifo->writeStreamString(tools::time::byFormat("%H:%M:%S\n", tools::time::now()));
		fifo->flush();

	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << ex.line << endl;
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

		io::fifo fifo1;
		fifo1.open();

		///< write first to avoid deadlock due to io::fifo is threadsafe and here one object is used
		j.addNRun(&threadWrite, (void *)&fifo1);
		tools::os::sleep(1);
		j.addNRun(&threadRead, (void *)&fifo1);

		j.wait();

		io::fifo fifo2 = fifo1;
		//or clone
		//fifo2.clone(fifo1);

		///< use a copy, so no need to keep an order
		j.addNRun(&threadRead, (void *)&fifo1);
		j.addNRun(&threadWrite, (void *)&fifo2);
		
		j.wait();

#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (string)ex << ex.line << endl;
	}

	return 0;
}
