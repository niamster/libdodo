/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/pcThreadCollection.h>
#include <libdodo/ioPipe.h>
#include <libdodo/ioFileFifo.h>
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

		io::channel *pipe = (io::channel *)data;

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

		io::channel *pipe = (io::channel *)data;

		pipe->writeStreamString(tools::time::byFormat("%H:%M:%S", tools::time::now()));
		pipe->flush();
		
		str = tools::filesystem::getFileContents("test.cc");
		
		pipe->writeStreamString(tools::string::ulToString(str.size()));
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
		
		cout << "\n~~using one pipe for the thread~~\n";

		io::pipe pipe1;
		pipe1.open();

		///< write first to avoid deadlock due to io::pipe is threadsafe and here one object is used
		j.addNRun(&threadWrite, (void *)dynamic_cast<io::channel *>(&pipe1));
		tools::os::sleep(1);
		j.addNRun(&threadRead, (void *)dynamic_cast<io::channel *>(&pipe1));

		j.wait();

		cout << "\n~~using original cloned pipe for one thread and copy for the second~~\n";

		io::pipe pipe2 = pipe1;
		//or clone
		//pipe2.clone(pipe1);

		///< use a copy, so no need to keep an order
		j.addNRun(&threadRead, (void *)dynamic_cast<io::channel *>(&pipe1));
		j.addNRun(&threadWrite, (void *)dynamic_cast<io::channel *>(&pipe2));
		
		j.wait();
		
		cout << "\n~~using FIFO file, opened to read for one thread and to write for the second~~\n";

		io::file::fifo fifo1;
		fifo1.open("fifo.file", io::file::FIFO_OPENMODE_READ_OPENNONBLOCK);
		io::file::fifo fifo2;
		fifo2.open("fifo.file", io::file::FIFO_OPENMODE_WRITE);

		///< use a copy, so no need to keep an order
		j.addNRun(&threadWrite, (void *)dynamic_cast<io::channel *>(&fifo2));
		j.addNRun(&threadRead, (void *)dynamic_cast<io::channel *>(&fifo1));
		
		j.wait();
#endif
	}
	catch (dodo::exception::basic ex)
	{
		cout << (dodoString)ex << ex.line << endl;
	}

	return 0;
}
