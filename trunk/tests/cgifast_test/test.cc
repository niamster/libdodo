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
#include <libdodo/cgiBasicServer.h>
#include <libdodo/pcSyncThreadDataSingle.h>

#include <iostream>

using namespace std;

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

	///increment counter in shared memory
	int *inc = (int *)sh.acquire();
	sh.release();

	exchange *io = cgit;
	io->writeStreamString("The headers thould be already printed successfully.<br>");

#ifndef FASTCGI_EXT

	io->writeStreamString("No fastCGI extension was compiled!<br>");

#endif

	fcgi.writeStreamString("counter: " + tools::string::iToString(*inc) + "<br>");

	fcgi.writeStreamString("GET[\"argument\"]: " + cgit.GET["argument"] + "<br>");
	fcgi.writeStreamString("POST[\"hidden\"]: " + cgit.POST["hidden"] + "<br>");
	fcgi.writeStreamString("POST[\"test\"]: " + cgit.POST["test"] + "<br>");
	fcgi.writeStreamString("ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]: " + cgit.ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING] + "<br>");
	fcgi.writeStreamString("COOKIES[\"test\"]: " + cgit.COOKIES["test"] + "<br>");
	fcgi.writeStreamString("FILES[\"file\"].size: " + tools::string::iToString(cgit.FILES["file"].size) + "<br>");
	fcgi.writeStreamString("tpl::processor:<br>");

	try
	{
		processor cgip;

		cgip.assign("test", "hoho");
		cgip.assign("one", "one");

		dodoStringArray strarr;
		strarr.push_back("one");
		strarr.push_back("two");
		strarr.push_back("three");
		cgip.assign("strarr", strarr);

		dodoStringMap strmap;
		strmap["one"] = "one";
		strmap["two"] = "two";
		strmap["three"] = "three";
		cgip.assign("strmap", strmap);

		dodoArray<dodoStringMap> strmaparr;
		strmaparr.push_back(strmap);
		strmap["one"] = "three";
		strmaparr.push_back(strmap);
		cgip.assign("strmaparr", strmaparr);

		cgit.printStream(cgip.processFile("test.tpl"));
	}
	catch (dodo::exception::basic ex)
	{
		fcgi.writeStreamString(ex.baseErrstr + " " + tools::string::lToString(ex.line));
	}

	fcgi.writeStreamString("<br>");
	
	inc = (int *)sh.acquire();
	(*inc)++;
	sh.release();
}

int main(int argc, char **argv)
{

	try
	{
		int *shared = new int (1);
		sh.set((void *)shared);

#ifdef FASTCGI_EXT

		using namespace cgi::fast;

		server c(5);
		if (!c.isFastCgi())
		{
			cout << "Not a fastCGI.";
			return 1;
		}

#else
		using namespace cgi::basic;

		server c;

#endif

		c.serve(&cgif);

		delete shared;
	}
	catch (dodo::exception::basic &ex)
	{
		cout << endl << ex.baseErrstr << endl << ex.line << endl << ex.baseErrno << endl << endl;
	}

	return 0;
}
