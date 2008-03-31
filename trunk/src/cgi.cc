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

const char *cgi::environmentStatements[] =
{
	"REQUEST_METHOD",
	"REQUEST_URI",
	"QUERY_STRING",
	"CONTENT_TYPE",
	"CONTENT_LENGTH",
	"CONTENT_TRANSFER_ENCODING",
	"HTTP_HOST",
	"HTTP_USER_AGENT",
	"HTTP_COOKIE",
	"HTTP_ACCEPT",
	"HTTP_ACCEPT_LANGUAGE",
	"HTTP_ACCEPT_ENCODING",
	"HTTP_ACCEPT_CHARSET",
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
	
__cgiFile::__cgiFile() : size(0),
						error(CGI_POSTFILEERR_NO_FILE)
{
}

//-------------------------------------------------------------------

__cookie::__cookie(const dodoString &a_name,
					 const dodoString &a_value,
					 const dodoString &a_exDate,
					 const dodoString &a_path,
					 const dodoString &a_domain,
					 bool a_secure) : name(a_name),
									  value(a_value),
									  exDate(a_exDate),
									  path(a_path),
									  domain(a_domain),
									  secure(a_secure)
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

cgi::cgi(dodoStringMap &headers,
		 bool silent,
		 bool autocleanContent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false)
#ifdef FCGI_EXT
										 ,
										 cgiFastSet(false)
#endif

{
	cgiIO = new ioSTD;

	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();

	detectMethod();

	makeContent();
	makePost();

	if (autocleanContent)
		content.clear();

	make(COOKIES, ENVIRONMENT[CGI_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

cgi::cgi(bool silent,
		 bool autocleanContent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false)
#ifdef FCGI_EXT
										 ,
										 cgiFastSet(false)
#endif

{
	cgiIO = new ioSTD;

	initHeaders(__dodostringmap__);

	if (!silent)
		printHeaders();

	makeEnv();

	detectMethod();

	makeContent();
	makePost();

	if (autocleanContent)
		content.clear();

	make(COOKIES, ENVIRONMENT[CGI_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

#ifdef FCGI_EXT

cgi::cgi(ioCgiFastExchange    *a_cf,
		 bool silent,
		 bool autocleanContent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 cgiFastSet(true),
										 cgiIO(a_cf),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false)

{
	initHeaders(__dodostringmap__);

	if (!silent)
		printHeaders();

	makeEnv();

	detectMethod();

	makeContent();
	makePost();

	if (autocleanContent)
		content.clear();

	make(COOKIES, ENVIRONMENT[CGI_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CGI_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

cgi::cgi(ioCgiFastExchange    *a_cf,
		 dodoStringMap &headers,
		 bool silent,
		 bool autocleanContent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 cgiFastSet(true),
										 cgiIO(a_cf),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false)

{
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();

	detectMethod();

	makeContent();
	makePost();

	if (autocleanContent)
		content.clear();

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
	std::map<dodoString, __cgiFile>::iterator i(FILES.begin()), j(FILES.end());
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
	dodoStringArray getPair = tools::explode(tools::decodeURL(string), delim);

	dodoStringArray::iterator l(getPair.begin()), m(getPair.end());

	dodoStringArray temp;

	for (; l != m; ++l)
	{
		temp = tools::explode(*l, "=");
		if (temp.size() > 1)
			val.insert(temp[0], temp[1]);
	}
}

//-------------------------------------------------------------------

void
cgi::makeEnv()
{
	char *env;

	for (int i = 0; i < HTTP_ENVIRONMENT_SIZE; ++i)
	{
#ifdef FCGI_EXT
		if (cgiFastSet)
			env = ((ioCgiFastExchange *)cgiIO)->getenv(environmentStatements[i]);
		else
#endif
		env = getenv(environmentStatements[i]);

		ENVIRONMENT[i] = env == NULL ? "NULL" : env;
	}
}

//-------------------------------------------------------------------

void
cgi::initHeaders(dodoStringMap &headers)
{
	if (headers.size() > 0)
	{
		dodoStringMap::iterator i(headers.begin()), j(headers.end());
		for (; i != j; ++i)
			HEADERS.insert(i->first, i->second);
	}
	else
	{
		HEADERS.insert("Content-type", "text/html");
		HEADERS.insert("X-Powered-By", PACKAGE_NAME "/" PACKAGE_VERSION);
	}
}

//-------------------------------------------------------------------

void
cgi::printHeaders() const
{
	if (headersPrinted)
		return;

	headersPrinted = true;

	dodoStringMap::const_iterator i(HEADERS.begin()), j(HEADERS.end());
	for (; i != j; ++i)
		cgiIO->writeStreamString(i->first + ": " + i->second + "\r\n");

	if (cookies.size() > 0)
	{
		dodoList<__cookie>::const_iterator i(cookies.begin()), j(cookies.end());
		for (; i != j; ++i)
		{
			cgiIO->writeStreamString("Set-Cookie: ");
			cgiIO->writeStreamString(i->name + "=" + i->value + "; ");
			if (i->path.size() > 0)
				cgiIO->writeStreamString("path=" + i->path + "; ");
			if (i->exDate.size() > 0)
				cgiIO->writeStreamString("expires=" + i->exDate + "; ");
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
		
						temp0 = i->find("Content-Type: ", temp1);
						temp0 += 14;
						temp1 = i->find("\n", temp0);
						file.type = i->substr(temp0, temp1 - temp0);
						temp1 += 3;
		
						file.size = i->size() - temp1 - 2;
		
						if (postFilesInMem)
							file.buf.assign(i->c_str() + temp1, file.size);
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
								FILES.insert(post_name, file);
		
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
		
						FILES.insert(post_name, file);
					}
					else
					{	
						if ((temp0 = i->find("name=\"")) == dodoString::npos)
							continue;
						
						temp0 += 6;
						temp1 = i->find("\"", temp0);
		
						POST.insert(i->substr(temp0, temp1 - temp0), i->substr(temp1 + 5, i->size() - temp1 - 7));
					}
				}
			}
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
			   const dodoString &exDate,
			   const dodoString &path,
			   const dodoString &domain,
			   bool secure)
{
	__cookie temp(secure);
	temp.name = name;
	temp.value = tools::encodeURL(value);
	temp.exDate = exDate;
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

