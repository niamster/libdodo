/***************************************************************************
 *            cgi.cc
 *
 *  Sat Sep  18 17:18:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <libdodo/cgi.h>

using namespace dodo;

const char *cgi::environmentStatements[] ={ "REQUEST_METHOD",
	"REQUEST_URI",
	"QUERY_STRING",
	"CONTENT_TYPE",
	"CONTENT_LENGTH",
	"CONTENT_TRANSFER_ENCODING",
	"HTTP_AUTHORIZATION",
	"HTTP_HOST",
	"HTTP_USER_AGENT",
	"HTTP_COOKIE",
	"HTTP_DATE",
	"HTTP_IF_MODIFIED_SINCE",
	"HTTP_ACCEPT",
	"HTTP_ACCEPT_LANGUAGE",
	"HTTP_ACCEPT_ENCODING",
	"HTTP_ACCEPT_CHARSET",
	"HTTP_ACCEPT_RANGES",
	"HTTP_KEEP_ALIVE",
	"HTTP_CONNECTION",
	"HTTP_REFERER",
	"HTTP_VIA",
	"HTTP_X_FORWARDED_FOR",
	"REMOTE_ADDR",
	"REMOTE_PORT",
	"REDIRECT_STATUS",
	"REDIRECT_QUERY_STRING",
	"REDIRECT_URL",
	"GATEWAY_INTERFACE",
	"PATH",
	"SERVER_SIGNATURE",
	"SERVER_SOFTWARE",
	"SERVER_NAME",
	"SERVER_ADDR",
	"SERVER_PORT",
	"SERVER_ADMIN",
	"SERVER_PROTOCOL",
	"SCRIPT_FILENAME",
	"SCRIPT_NAME",
};

//-------------------------------------------------------------------

const dodoString cgi::responseHeaderStatements[] = { "Accept-Ranges",
		"Age",
		"Allow",
		"Cache-Control",
		"Content-Encoding",
		"Content-Language",
		"Content-Length",
		"Content-Location",
		"Content-Disposition",
		"Content-MD5",
		"Content-Range",
		"Content-Type",
		"Date",
		"Last-Modified",
		"Location",
		"Server",
		"WWW-Authenticate",
		"X-Powered-By",
};

const dodoString cgi::responseStatusStatements[] = { "HTTP/1.1 100 Continue\r\n",
		"HTTP/1.1 101 Switching Protocols\r\n",
		"HTTP/1.1 200 OK\r\n",
		"HTTP/1.1 201 Created\r\n",
		"HTTP/1.1 202 Accepted\r\n",
		"HTTP/1.1 203 Non-Authoritative Information\r\n",
		"HTTP/1.1 204 No Content\r\n",
		"HTTP/1.1 205 Reset Content\r\n",
		"HTTP/1.1 206 Partial Content\r\n",
		"HTTP/1.1 300 Multiple Choices\r\n",
		"HTTP/1.1 301 Moved Permanently\r\n",
		"HTTP/1.1 302 Found\r\n",
		"HTTP/1.1 303 See Other\r\n",
		"HTTP/1.1 304 Not Modified\r\n",
		"HTTP/1.1 305 Use Proxy\r\n",
		"HTTP/1.1 307 Temporary Redirect\r\n",
		"HTTP/1.1 400 Bad Request\r\n",
		"HTTP/1.1 401 Unauthorized\r\n",
		"HTTP/1.1 402 Payment Required\r\n",
		"HTTP/1.1 403 Forbidden\r\n",
		"HTTP/1.1 404 Not Found\r\n",
		"HTTP/1.1 405 Method Not Allowed\r\n",
		"HTTP/1.1 406 Not Acceptable\r\n",
		"HTTP/1.1 407 Proxy Authentication Required\r\n",
		"HTTP/1.1 408 Request Timeout\r\n",
		"HTTP/1.1 409 Conflict\r\n",
		"HTTP/1.1 410 Gone\r\n",
		"HTTP/1.1 411 Length Required\r\n",
		"HTTP/1.1 412 Precondition Failed\r\n",
		"HTTP/1.1 413 Request Entity Too Large\r\n",
		"HTTP/1.1 414 Request-URI Too Long\r\n",
		"HTTP/1.1 415 Unsupported Media Type\r\n",
		"HTTP/1.1 416 Requested Range Not Satisfiable\r\n",
		"HTTP/1.1 417 Expectation Failed\r\n",
		"HTTP/1.1 500 Internal Server Error\r\n",
		"HTTP/1.1 501 Not Implemented\r\n",
		"HTTP/1.1 502 Bad Gateway\r\n",
		"HTTP/1.1 503 Service Unavailable\r\n",
		"HTTP/1.1 504 Gateway Timeout\r\n",
		"HTTP/1.1 505 HTTP Version Not Supported\r\n",
};

//-------------------------------------------------------------------
	
__cgiFile::__cgiFile() : size(0),
						error(CGI_POSTFILEERR_NO_FILE)
{
}

//-------------------------------------------------------------------

__cookie::__cookie(const dodoString &a_name,
					 const dodoString &a_value,
					 const dodoString &a_expires,
					 const dodoString &a_path,
					 const dodoString &a_domain,
					 bool a_secure) : name(a_name),
									  value(a_value),
									  expires(a_expires),
									  path(a_path),
									  domain(a_domain),
									  secure(a_secure)
{
}

//-------------------------------------------------------------------

__cookie::__cookie() : secure(false)
{
}

//-------------------------------------------------------------------

__cookie::__cookie(bool a_secure) : secure(a_secure)
{
}

//-------------------------------------------------------------------

cgi::cgi(cgi &ct)
{
}

//-------------------------------------------------------------------

cgi::cgi(dodoMap<short, dodoString> &headers,
		 bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CGI_STATUSCODE_OK)
#ifdef FCGI_EXT
										 ,
										 cgiFastSet(false)
#endif

{
	authInfo.type = CGI_AUTHTYPE_NONE;
	
	cgiIO = new ioSTD;

	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();
	
	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[CGI_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

cgi::cgi(bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CGI_STATUSCODE_OK)
#ifdef FCGI_EXT
										 ,
										 cgiFastSet(false)
#endif

{
	authInfo.type = CGI_AUTHTYPE_NONE;
	
	cgiIO = new ioSTD;

	dodoMap<short, dodoString> headers;
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();
	
	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[CGI_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

#ifdef FCGI_EXT

cgi::cgi(cgiFastExchange    *a_cf,
		 bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 cgiFastSet(true),
										 cgiIO(a_cf),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CGI_STATUSCODE_OK)

{
	authInfo.type = CGI_AUTHTYPE_NONE;
	
	dodoMap<short, dodoString> headers;
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();
	
	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[CGI_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

cgi::cgi(cgiFastExchange    *a_cf,
		dodoMap<short, dodoString> &headers,
		 bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 cgiFastSet(true),
										 cgiIO(a_cf),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CGI_STATUSCODE_OK)

{
	authInfo.type = CGI_AUTHTYPE_NONE;
			
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();
	
	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[CGI_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]);
}


#endif

//-------------------------------------------------------------------

cgi::~cgi()
{
	printHeaders();

	if (autocleanFiles)
		cleanTmp();

#ifdef FCGI_EXT
	if (!cgiFastSet)
		delete cgiIO;
#endif
}

//-------------------------------------------------------------------

void
cgi::flush()
{
	cgiIO->flush();
}

//-------------------------------------------------------------------

void
cgi::printStream(const dodoString &buf)
{
	printHeaders();

	cgiIO->writeStreamString(buf);
}

//-------------------------------------------------------------------

void
cgi::print(const dodoString &buf)
{
	printHeaders();

	cgiIO->outSize = buf.size();
	cgiIO->writeString(buf);
}

//-------------------------------------------------------------------

void 
cgi::makeAuth()
{
	if (stringTools::contains(ENVIRONMENT[CGI_ENVIRONMENT_HTTPAUTHORIZATION], "Basic"))
	{
		dodoStringArray arr = tools::explode(tools::decodeBase64(stringTools::trim(ENVIRONMENT[CGI_ENVIRONMENT_HTTPAUTHORIZATION].substr(6),' ')), ":", 2);
		
		authInfo.type = CGI_AUTHTYPE_BASIC;
		authInfo.user = arr[0];
		if (arr.size() == 2)
			authInfo.password = arr[1];
	}
	else 
	{
		if (stringTools::contains(ENVIRONMENT[CGI_ENVIRONMENT_HTTPAUTHORIZATION], "Digest"))
		{
			
		}
	}
}

//-------------------------------------------------------------------

void 
cgi::requestBasicAuthentification(const dodoString &realm)
{
	returnCode = CGI_STATUSCODE_UNAUTHORIZED;
	
	HEADERS.insert(make_pair(CGI_RESPONSEHEADER_WWWAUTHENTICATE, dodoString("Basic realm=\"") + realm + "\""));
}

//-------------------------------------------------------------------

__authInfo 
cgi::getAuthentificationInfo()
{
	__authInfo info = {authInfo.user, authInfo.type};
	
	return info;
}

//-------------------------------------------------------------------

bool 
cgi::checkBasicAuthentification(const dodoString &user, 
								const dodoString &password)
{
	return (stringTools::equal(user,authInfo.user) && stringTools::equal(password,authInfo.password));
}

//-------------------------------------------------------------------

dodoString
cgi::getContent()
{
	return content;
}

//-------------------------------------------------------------------

void
cgi::clearContent()
{
	content.clear();
}

//-------------------------------------------------------------------

void
cgi::cleanTmp()
{
	dodoMap<dodoString, __cgiFile>::iterator i(FILES.begin()), j(FILES.end());
	for (; i != j; ++i)
	{
		if (!postFilesInMem)
			unlink(i->second.tmp_name.c_str());
	}
}

//-------------------------------------------------------------------

void
cgi::detectMethod()
{
	if (stringTools::iequal(ENVIRONMENT[CGI_ENVIRONMENT_REQUESTMETHOD], "GET"))
		method = CGI_REQUESTMETHOD_GET;
	else
	{
		if (stringTools::iequal(ENVIRONMENT[CGI_ENVIRONMENT_REQUESTMETHOD], "POST") && ENVIRONMENT[CGI_ENVIRONMENT_REQUESTMETHOD].empty())
			method = CGI_REQUESTMETHOD_POST;
		else
			method = CGI_REQUESTMETHOD_GET_POST;
	}
}

//-------------------------------------------------------------------

int
cgi::getMethod() const
{
	return method;
}

//-------------------------------------------------------------------

void
cgi::make(dodoStringMap &val,
		  const dodoString &string,
		  const char       *delim)
{
	dodoStringArray getPair = tools::explode(tools::decodeUrl(string), delim);

	dodoStringArray::iterator l(getPair.begin()), m(getPair.end());

	dodoStringArray temp;

	for (; l != m; ++l)
	{
		temp = tools::explode(*l, "=");
		if (temp.size() > 1)
			val.insert(make_pair(temp[0], temp[1]));
	}
}

//-------------------------------------------------------------------

void
cgi::makeEnv()
{
	char *env;

	for (int i = 0; i < CGI_ENVIRONMENTSTATEMENTS; ++i)
	{
#ifdef FCGI_EXT
		if (cgiFastSet)
			env = ((cgiFastExchange *)cgiIO)->getenv(environmentStatements[i]);
		else
#endif
		env = getenv(environmentStatements[i]);

		ENVIRONMENT[i] = env == NULL ? "NULL" : env;
	}
	
	ENVIRONMENT[CGI_ENVIRONMENT_CONTENTTYPE] = tools::explode(ENVIRONMENT[CGI_ENVIRONMENT_CONTENTTYPE], ";", 2).front();
}

//-------------------------------------------------------------------

void
cgi::initHeaders(dodoMap<short, dodoString> &headers)
{
	if (headers.size() > 0)
	{
		HEADERS.insert(headers.begin(), headers.end());
	}
	else
	{
		HEADERS.insert(make_pair(CGI_RESPONSEHEADER_CONTENTTYPE, dodoString("text/html")));
		HEADERS.insert(make_pair(CGI_RESPONSEHEADER_XPOWEREDBY, dodoString(PACKAGE_NAME "/" PACKAGE_VERSION)));
	}
}

//-------------------------------------------------------------------

void 
cgi::setResponseStatus(short code)
{
	if (code <= CGI_STATUSCODE_HTTPVERSIONNOTSUPPORTED)
		returnCode = code;
	else
		throw baseEx(ERRMODULE_CGI, CGIEX_SETRESPONSESTATUS, ERR_LIBDODO, CGIEX_WRONGSTATUSCODE, CGIEX_WRONGSTATUSCODE_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
cgi::printHeaders() const
{
	if (headersPrinted)
		return;

	headersPrinted = true;

	cgiIO->writeStreamString(responseStatusStatements[returnCode]);
	
	dodoMap<short, dodoString>::const_iterator i(HEADERS.begin()), j(HEADERS.end());
	for (; i != j; ++i)
		cgiIO->writeStreamString(responseHeaderStatements[i->first] + ": " + i->second + "\r\n");

	if (cookies.size() > 0)
	{
		dodoList<__cookie>::const_iterator i(cookies.begin()), j(cookies.end());
		for (; i != j; ++i)
		{
			cgiIO->writeStreamString("Set-Cookie: ");
			cgiIO->writeStreamString(i->name + "=" + i->value + "; ");
			if (i->path.size() > 0)
				cgiIO->writeStreamString("path=" + i->path + "; ");
			if (i->expires.size() > 0)
				cgiIO->writeStreamString("expires=" + i->expires + "; ");
			if (i->domain.size() > 0)
				cgiIO->writeStreamString("domain=" + i->domain + "; ");
			if (i->secure)
				cgiIO->writeStreamString("secure");
			cgiIO->writeStreamString("\r\n");
		}
	}

	cgiIO->writeStreamString("\r\n");
	cgiIO->flush();
}

//-------------------------------------------------------------------

void
cgi::makeContent()
{
	unsigned long inSize = stringTools::stringToUL(ENVIRONMENT[CGI_ENVIRONMENT_CONTENTLENGTH]);

	if (inSize <= 0)
		return ;

	cgiIO->inSize = inSize;
	
	cgiIO->readString(content);
}

//-------------------------------------------------------------------

void
cgi::makePost()
{
	if (content.size() == 0)
		return ;

	if (!stringTools::iequal(ENVIRONMENT[CGI_ENVIRONMENT_REQUESTMETHOD], "POST"))
		return ;

	if (stringTools::iequal(ENVIRONMENT[CGI_ENVIRONMENT_CONTENTTYPE], "application/x-www-form-urlencoded"))
	{
		make(POST, content);
		
		content.clear();
	}
	else
	{
		if (stringTools::contains(ENVIRONMENT[CGI_ENVIRONMENT_CONTENTTYPE], "multipart/form-data"))
		{
			if (stringTools::iequal(ENVIRONMENT[CGI_ENVIRONMENT_CONTENTTRANSFERENCODING], "base64"))
				content = tools::decodeBase64(content);
	
			unsigned int temp0;
			temp0 = ENVIRONMENT[CGI_ENVIRONMENT_CONTENTTYPE].find("boundary=");
			dodoStringArray postPartd = tools::explode(content, "--" + ENVIRONMENT[CGI_ENVIRONMENT_CONTENTTYPE].substr(temp0 + 9));
	
			dodoStringArray::iterator i(postPartd.begin() + 1), j(postPartd.end());
	
			unsigned int temp1;
			char *ptr;
			int fd;
			unsigned short pathLength = postFilesTmpDir.size() + 18;
			FILE *fp;

			for (; i != j; ++i)
			{
				if (stringTools::iequal(i->substr(0, 2), "--"))
					break;
				else
				{
					if (i->find("filename") != dodoString::npos)
					{
						if ((temp0 = i->find("name=\"")) == dodoString::npos)
							continue;
						temp0 += 6;
						temp1 = i->find("\"", temp0);
		
						dodoString post_name = i->substr(temp0, temp1 - temp0);
		
						__cgiFile file;
		
						temp0 = i->find("filename=\"", temp1);
						temp0 += 10;
						temp1 = i->find("\"", temp0);
						if (temp0 == temp1)
							continue;
		
						file.name = i->substr(temp0, temp1 - temp0);
		
						temp0 = stringTools::find(*i, "Content-Type: ", temp1, true);
						temp0 += 14;
						temp1 = i->find("\n", temp0);
						file.type = tools::explode(i->substr(temp0, temp1 - temp0), ";")[0];
						temp1 += 3;
		
						file.size = i->size() - temp1 - 2;
		
						if (postFilesInMem)
							file.data.assign(i->c_str() + temp1, file.size);
						else
						{
							file.error = CGI_POSTFILEERR_NONE;
		
							ptr = new char[pathLength];
							strncpy(ptr, dodoString(postFilesTmpDir + FILE_DELIM + dodoString("dodo_post_XXXXXX")).c_str(), pathLength);
							fd = mkstemp(ptr);
							if (fd == -1)
							{
								delete [] ptr;
		
								file.error = CGI_POSTFILEERR_BAD_FILE_NAME;
								FILES.insert(make_pair(post_name, file));
		
								continue;
							}
		
							file.tmp_name = ptr;
		
							delete [] ptr;
		
							fp = fdopen(fd, "w+");
							if (fp == NULL)
							{
								switch (errno)
								{
									case EACCES:
									case EISDIR:
		
										file.error = CGI_POSTFILEERR_ACCESS_DENY;
		
										break;
		
									case ENAMETOOLONG:
									case ENOTDIR:
		
										file.error = CGI_POSTFILEERR_BAD_FILE_NAME;
		
										break;
		
									case ENOMEM:
		
										file.error = CGI_POSTFILEERR_NO_SPACE;
		
										break;
									
									default:
										
										throw baseEx(ERRMODULE_CGI, CGIEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
								}
							}
							else
							{
								if (fwrite(i->c_str() + temp1, file.size, 1, fp) == 0)
								{
									if (errno == ENOMEM)
										file.error = CGI_POSTFILEERR_NO_SPACE;
									else
									{
										if (fclose(fp) != 0)
											throw baseEx(ERRMODULE_CGI, CGIEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
									
										throw baseEx(ERRMODULE_CGI, CGIEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
									}
								}
		
								if (fclose(fp) != 0)
									throw baseEx(ERRMODULE_CGI, CGIEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
							}
						}
		
						FILES.insert(make_pair(post_name, file));
					}
					else
					{	
						if ((temp0 = i->find("name=\"")) == dodoString::npos)
							continue;
						
						temp0 += 6;
						temp1 = i->find("\"", temp0);
		
						POST.insert(make_pair(i->substr(temp0, temp1 - temp0), i->substr(temp1 + 5, i->size() - temp1 - 7)));
					}
				}
			}
			
			content.clear();
		}
	}
}

//-------------------------------------------------------------------

const dodoStringMap &
cgi::operator[](short method)
{
	if (method == CGI_REQUESTMETHOD_POST)
		return POST;
	else
		return GET;
}

//-------------------------------------------------------------------

dodoString
cgi::request(const dodoString &varName,
			 short first)
{
	dodoString met0 = GET[varName];
	dodoString met1 = POST[varName];

	if (first == CGI_REQUESTMETHOD_GET)
		if (met0.size() != 0)
			return met0;
		else
			return met1;
	else
	if (met0.size() != 0)
		return met1;
	else
		return met0;

}

//-------------------------------------------------------------------

void
cgi::setCookie(const dodoString &name,
			   const dodoString &value,
			   const dodoString &expires,
			   const dodoString &path,
			   const dodoString &domain,
			   bool secure)
{
	__cookie temp(secure);
	temp.name = name;
	temp.value = tools::encodeUrl(value);
	temp.expires = expires;
	temp.path = path;
	temp.domain = domain;
	cookies.push_back(temp);
}

//-------------------------------------------------------------------

void
cgi::setCookie(const __cookie &cookie)
{
	cookies.push_back(cookie);
}

//-------------------------------------------------------------------

