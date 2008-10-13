/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/exceptionBasic.h>
#include <libdodo/cgiDialogue.h>
#include <libdodo/dataTplProcessor.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/cgiFastServer.h>
#include <libdodo/pcSyncThreadDataSingle.h>

#include <iostream>

using namespace std;

#ifdef FASTCGI_EXT

using namespace dodo;
using namespace data::tpl;
using cgi::exchange;
using dodo::pc::sync::thread::data::single;

single sh;

void
cgif(exchange &fcgi)
{
	using namespace cgi;

	dialogue cgit(fcgi, true);

	cgit.setCookie("test", "Ni@m");

	/// will print headers if they haven't been printed before
	/// headers are printed also if print was called but headers hadn't been called before
	cgit.printHeaders();

	///increment counter in shared memory
	int *inc = (int *)sh.acquire();
	(*inc)++;
	sh.release();

	fcgi.writeStreamString(tools::string::iToString(*inc) + "<br>");
	fcgi.writeStreamString(cgit.GET["a"] + "<br>");
	fcgi.writeStreamString(cgit.POST["hidden"] + "<br>");
	fcgi.writeStreamString(cgit.POST["test"] + "<br>");
	fcgi.writeStreamString(cgit.ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING] + "<br>");
	fcgi.writeStreamString(cgit.COOKIES["test"] + "<br>");
	fcgi.writeStreamString(tools::string::iToString(cgit.FILES["file"].size) + "<br>");
	fcgi.writeStreamString("<br>");

	try
	{
		processor cgip;

		cgip.assign("test", "hoho");
		cgip.assign("show", "That's works!");
		cgip.assign("one", "one");

		dodoStringArray arr;
		arr.push_back("one");
		arr.push_back("two");
		arr.push_back("three");
		cgip.assign("arr", arr);

		dodoStringMap arr1;
		arr1["one"] = "one";
		arr1["two"] = "two";
		arr1["three"] = "three";
		cgip.assign("arr1", arr1);

		dodoArray<dodoStringMap> arr2;
		arr2.push_back(arr1);
		arr1["one"] = "three";
		arr2.push_back(arr1);
		cgip.assign("arr2", arr2);

		cgit.printStream(cgip.process("test.tpl"));
	}
	catch (dodo::exception::basic ex)
	{
		fcgi.writeStreamString(ex.baseErrstr + " " + tools::string::lToString(ex.line));
	}

	fcgi.writeStreamString("<br>");
}

#endif

int main(int argc, char **argv)
{
#ifdef FASTCGI_EXT

	try
	{
		int *shared = new int (1);
		sh.set((void *)shared);

		using namespace cgi::fast;

		server cf(5, true, 10);
		if (!cf.isFastCgi())
		{
			cout << "Not a fastCGI.";
			cout.flush();
		}

		cf.listen(&cgif);

		delete shared;
	}
	catch (dodo::exception::basic &ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl << endl;
	}

#else

	cout << "No fastCGI extension was compiled!";

#endif

	return 0;
}
