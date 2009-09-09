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
handler(exchange &ex)
{
	using namespace cgi;

	dialogue d(ex, true);

	d.setCookie(cookie("test", "Ni@m"));

	///increment counter in shared memory
	int *inc = (int *)sh.acquire();
	sh.release();

	exchange *io = d;
	io->writeString("The headers thould be already printed successfully.<br>");

#ifndef FASTCGI_EXT
	io->writeString("No fastCGI extension was compiled!<br>");
#endif

	ex.writeString("counter: " + tools::string::iToString(*inc) + "<br>");

	ex.writeString("GET[\"argument\"]: " + d.GET["argument"] + "<br>");
	ex.writeString("POST[\"hidden\"]: " + d.POST["hidden"] + "<br>");
	ex.writeString("POST[\"test\"]: " + d.POST["test"] + "<br>");
	ex.writeString("ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]: " + d.ENVIRONMENT[cgi::ENVIRONMENT_QUERYSTRING] + "<br>");
	ex.writeString("COOKIES[\"test\"]: " + d.COOKIES["test"] + "<br>");
	ex.writeString("FILES[\"file\"].size: " + tools::string::iToString(d.FILES["file"].size) + "<br>");
	ex.writeString("tpl::processor:<br>");

	try
	{
		processor p;

		p.assign("test", "test");
		p.assign("one", "one");

		dodoStringArray strarr;
		strarr.push_back("one");
		strarr.push_back("two");
		strarr.push_back("three");
		p.assign("strarr", strarr);

		dodoStringMap strmap;
		strmap["one"] = "one";
		strmap["two"] = "two";
		strmap["three"] = "three";
		p.assign("strmap", strmap);

		dodoArray<dodoStringMap> strmaparr;
		strmaparr.push_back(strmap);
		strmap["one"] = "three";
		strmaparr.push_back(strmap);
		p.assign("strmaparr", strmaparr);

		p.processFile("test.tpl", *io);
	}
	catch (dodo::exception::basic ex)
	{
		ex.writeString(ex.errStr + " " + tools::string::lToString(ex.line));
	}

	ex.writeString("<br>");

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
		c.serve(&handler);

		delete shared;
	}
	catch (dodo::exception::basic &ex)
	{
		cout << (dodoString)ex << "\t" << ex.line << "\t" << ex.file << endl;
	}

	return 0;
}
