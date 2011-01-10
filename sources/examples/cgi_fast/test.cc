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
using namespace pc;

sync::thread protector;
sync::data::object object(protector);

void
handler(exchange &ex)
{
    using namespace cgi;

    dialogue d(ex, POST_SIZE_LIMIT, true);

    d.setCookie(cookie("test", "Dmytro Milinevskyy"));

        ///increment counter in shared memory
    int *inc = (int *)object.acquire();
    object.release();

    exchange *io = d;
    io->writeString("The headers thould be already printed successfully.<br>");

#ifndef FASTCGI_EXT
    io->writeString("No fastCGI *etension was compiled!<br>");
#endif

    ex.writeString("counter: " + tools::string::iToString(*inc) + "<br>");

    ex.writeString("GET[\"argument\"]: " + d.GET["argument"] + "<br>");
    ex.writeString("POST[\"hidden\"]: " + d.POST["hidden"] + "<br>");
    ex.writeString("POST[\"test\"]: " + d.POST["test"] + "<br>");
    ex.writeString("ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]: " + d.ENVIRONMENT[cgi::ENVIRONMENT_QUERYSTRING] + "<br>");
    ex.writeString("COOKIES[\"test\"]: " + d.COOKIES["test"] + "<br>");
    ex.writeString("FILES[\"file\"].size: " + tools::string::iToString(d.FILES["file"].size) + "<br>");
    ex.writeString("tpl::processor:<br>");

    dodo_try {
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
    } dodo_catch (dodo::exception::basic *e)   {
        d.printString(e->errStr + " " + tools::string::lToString(e->line));
    }

    ex.writeString("<br>");

    inc = (int *)object.acquire();
    (*inc)++;
    object.release();
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    dodo_try {
        int *shared = new int(1);
        object.set((void *)shared);

#ifdef FASTCGI_EXT
        using namespace cgi::fast;

        server c(5, false);
        if (!c.isFastCgi()) {
            cout << "Not a fastCGI.";
            return 1;
        }
#else
        using namespace cgi::basic;
        server c;
#endif
        c.serve(&handler);

        delete shared;
    } dodo_catch (dodo::exception::basic *e)   {
        cout << (dodo::string)*e << "\t" << e->line << "\t" << e->file << endl;
    }

    return 0;
}
