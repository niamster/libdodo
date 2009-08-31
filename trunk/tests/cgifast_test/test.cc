/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
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

	cgit.setCookie(cookie("test", "Ni@m"));

	///increment counter in shared memory
	int *inc = (int *)sh.acquire();
	sh.release();

	exchange *io = cgit;
	io->writeString("The headers thould be already printed successfully.<br>");

#ifndef FASTCGI_EXT
	io->writeString("No fastCGI extension was compiled!<br>");
#endif

	fcgi.writeString("counter: " + tools::string::iToString(*inc) + "<br>");

	fcgi.writeString("GET[\"argument\"]: " + cgit.GET["argument"] + "<br>");
	fcgi.writeString("POST[\"hidden\"]: " + cgit.POST["hidden"] + "<br>");
	fcgi.writeString("POST[\"test\"]: " + cgit.POST["test"] + "<br>");
	fcgi.writeString("ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]: " + cgit.ENVIRONMENT[cgi::ENVIRONMENT_QUERYSTRING] + "<br>");
	fcgi.writeString("COOKIES[\"test\"]: " + cgit.COOKIES["test"] + "<br>");
	fcgi.writeString("FILES[\"file\"].size: " + tools::string::iToString(cgit.FILES["file"].size) + "<br>");
	fcgi.writeString("tpl::processor:<br>");

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

		cgip.processFile("test.tpl", *io);
	}
	catch (dodo::exception::basic ex)
	{
		fcgi.writeString(ex.errStr + " " + tools::string::lToString(ex.line));
	}

	fcgi.writeString("<br>");

	inc = (int *)sh.acquire();
	(*inc)++;
	sh.release();
}

int main(int argc, char **argv)
{

	try
	{
		int *shared = new int(1);
		sh.set((void *)shared);

#ifdef FASTCGI_EXT
		using namespace cgi::fast;

		server c(5,false);
		if (!c.isFastCgi())
		{
			cout << "Not a fastCGI.";
			return 1;
		}
#else
		using namespace cgi::basic;
		server c;
#endif
\
		c.serve(&cgif);

		delete shared;
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
