/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

using namespace dodo;
using namespace data::tpl;
using cgi::exchange;

using namespace std;

void
handler(exchange &ex)
{
    using namespace cgi;

    ///first type: pass headers and print them immediately
    //	dodoStringMap head;
    //	head[cgi::RESPONSE_HEADER_CONTENTTYPE] = "t*et/html";
    //	dialogue d(&*e, head, false);

        ///second type: use default headers and do not print them immediately
    dialogue d(ex, POST_SIZE_LIMIT, true);

    dodo::string user = d.authenticationResponse().user;

    if (d.GET["status"] == "forbidden") {
        d.setResponseStatus(cgi::STATUS_CODE_FORBIDDEN);

        d.printString("FORBIDDEN");

        return ;
    } else if (d.GET["status"] == "notfound")   {
        d.setResponseStatus(cgi::STATUS_CODE_NOTFOUND);

        d.printString("NOT FOUND");

        return ;
    }
    /**
     * A workaround for apache web server to get auth headers:
     *
     * RewriteEngine on
     * RewriteBase /
     * RewriteCond %{HTTP:Authorization}  ^(.*)
     * RewriteRule ^(.*)$ $1 [e=HTTP_AUTHORIZATION:%1
     */
    else if (d.GET["status"] == "basic_auth") {
        if (user.size() == 0 || !d.isAuthenticated("libdodo", "password")) {
            d.requestAuthentication("libdodo", cgi::AUTH_BASIC);

            return ;
        }
    } else if (d.GET["status"] == "digest_auth")   {
        if (user.size() == 0 || !d.isAuthenticated("libdodo", "password")) {
            d.requestAuthentication("libdodo", cgi::AUTH_DIGEST);

            return ;
        }
    }

    d.HEADERS[cgi::RESPONSE_HEADER_CONTENTTYPE] = "t*et/html";

    d.setCookie(cookie("test", "cookie"));

    exchange *io = d;
    io->writeString("The headers thould be already printed successfully.<br>");

    d.printString("User: " + user + "<br>");
    d.printString("GET[\"argument\"]: " + d.GET["argument"] + "<br>");
    d.printString("POST[\"hidden\"]: " + d.POST["hidden"] + "<br>");
    d.printString("POST[\"t*et\"]: " + d.POST["text"] + "<br>");
    d.printString("ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]: " + d.ENVIRONMENT[cgi::ENVIRONMENT_QUERYSTRING] + "<br>");
    d.printString("COOKIES[\"test\"]: " + d.COOKIES["test"] + "<br>");
    d.printString("FILES[\"file\"].size: " + tools::string::iToString(d.FILES["file"].size) + "<br>");
    d.printString("FILES[\"file\"].mime: " + d.FILES["file"].mime + "<br>");
    d.printString("charset: " + d.charset() + "<br>");
    d.printString("tpl::processor:<br>");

    dodo_try {
        processor p;

        p.assign("main", "inde->tpl");
        p.assign("test", "test");
        p.assign("show", "show");
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
        d.printString((dodo::string)*e + " " + tools::string::lToString(e->line) + " " + e->file + " " + e->message );
    }
}

int
main(int  argc UNUSED,
     char **argv UNUSED)
{
    using namespace cgi::basic;

    server s;

    s.serve(&handler);

    return 0;
}
