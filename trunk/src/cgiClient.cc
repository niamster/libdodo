/***************************************************************************
 *            cgiClient.cc
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

#include <libdodo/cgiClient.h>

using namespace dodo::cgi;

const char *client::environmentStatements[] ={ "REQUEST_METHOD",
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

const dodoString client::responseHeaderStatements[] = { "Accept-Ranges",
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

const dodoString client::responseStatusStatements[] = { "Status: 100 Continue\r\n",
		"Status: 101 Switching Protocols\r\n",
		"Status: 200 OK\r\n",
		"Status: 201 Created\r\n",
		"Status: 202 Accepted\r\n",
		"Status: 203 Non-Authoritative Information\r\n",
		"Status: 204 No Content\r\n",
		"Status: 205 Reset Content\r\n",
		"Status: 206 Partial Content\r\n",
		"Status: 300 Multiple Choices\r\n",
		"Status: 301 Moved Permanently\r\n",
		"Status: 302 Found\r\n",
		"Status: 303 See Other\r\n",
		"Status: 304 Not Modified\r\n",
		"Status: 305 Use Proxy\r\n",
		"Status: 307 Temporary Redirect\r\n",
		"Status: 400 Bad Request\r\n",
		"Status: 401 Unauthorized\r\n",
		"Status: 402 Payment Required\r\n",
		"Status: 403 Forbidden\r\n",
		"Status: 404 Not Found\r\n",
		"Status: 405 Method Not Allowed\r\n",
		"Status: 406 Not Acceptable\r\n",
		"Status: 407 Proxy Authentication Required\r\n",
		"Status: 408 Request Timeout\r\n",
		"Status: 409 Conflict\r\n",
		"Status: 410 Gone\r\n",
		"Status: 411 Length Required\r\n",
		"Status: 412 Precondition Failed\r\n",
		"Status: 413 Request Entity Too Large\r\n",
		"Status: 414 Request-URI Too Long\r\n",
		"Status: 415 Unsupported Media Type\r\n",
		"Status: 416 Requested Range Not Satisfiable\r\n",
		"Status: 417 Expectation Failed\r\n",
		"Status: 500 Internal Server Error\r\n",
		"Status: 501 Not Implemented\r\n",
		"Status: 502 Bad Gateway\r\n",
		"Status: 503 Service Unavailable\r\n",
		"Status: 504 Gateway Timeout\r\n",
		"Status: 505 HTTP Version Not Supported\r\n",
};

//-------------------------------------------------------------------
	
__clientFile::__clientFile() : size(0),
						error(CLIENT_POSTFILEERR_NO_FILE)
{
}

//-------------------------------------------------------------------

__clientCookie::__clientCookie(const dodoString &a_name,
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

__clientCookie::__clientCookie() : secure(false)
{
}

//-------------------------------------------------------------------

__clientCookie::__clientCookie(bool a_secure) : secure(a_secure)
{
}

//-------------------------------------------------------------------

client::client(client &ct)
{
}

//-------------------------------------------------------------------

client::client(dodoMap<short, dodoString> &headers,
		 bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CLIENT_STATUSCODE_OK)
#ifdef FASTCGI_EXT
										 ,
										 clientFastSet(false)
#endif

{
	authInfo.type = CLIENT_AUTHTYPE_NONE;
	
	cgiIO = new ioSTD;

	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();
	
	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[CLIENT_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CLIENT_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

client::client(bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CLIENT_STATUSCODE_OK)
#ifdef FASTCGI_EXT
										 ,
										 clientFastSet(false)
#endif

{
	authInfo.type = CLIENT_AUTHTYPE_NONE;
	
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

	make(COOKIES, ENVIRONMENT[CLIENT_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CLIENT_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

#ifdef FASTCGI_EXT

client::client(fastClientExchange    *a_cf,
		 bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 clientFastSet(true),
										 cgiIO(a_cf),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CLIENT_STATUSCODE_OK)

{
	authInfo.type = CLIENT_AUTHTYPE_NONE;
	
	dodoMap<short, dodoString> headers;
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();
	
	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[CLIENT_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CLIENT_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

client::client(fastClientExchange    *a_cf,
		dodoMap<short, dodoString> &headers,
		 bool silent,
		 bool a_autocleanFiles,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 clientFastSet(true),
										 cgiIO(a_cf),
										 autocleanFiles(a_autocleanFiles),
										 headersPrinted(false),
										  returnCode(CLIENT_STATUSCODE_OK)

{
	authInfo.type = CLIENT_AUTHTYPE_NONE;
			
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();
	
	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[CLIENT_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[CLIENT_ENVIRONMENT_QUERYSTRING]);
}


#endif

//-------------------------------------------------------------------

client::~client()
{
	printHeaders();

	if (autocleanFiles)
		cleanTmp();

#ifdef FASTCGI_EXT
	if (!clientFastSet)
		delete cgiIO;
#endif
}

//-------------------------------------------------------------------

void
client::flush()
{
	cgiIO->flush();
}

//-------------------------------------------------------------------

void
client::printStream(const dodoString &buf)
{
	printHeaders();

	cgiIO->writeStreamString(buf);
}

//-------------------------------------------------------------------

void
client::print(const dodoString &buf)
{
	printHeaders();

	cgiIO->outSize = buf.size();
	cgiIO->writeString(buf);
}

//-------------------------------------------------------------------

void 
client::makeAuth()
{
	dodoString &httpAuthorization = ENVIRONMENT[CLIENT_ENVIRONMENT_HTTPAUTHORIZATION];
	
	if (toolsString::contains(httpAuthorization, "Basic"))
	{
		dodoStringArray arr = tools::explode(tools::decodeBase64(toolsString::trim(httpAuthorization.substr(6),' ')), ":", 2);
		
		authInfo.type = CLIENT_AUTHTYPE_BASIC;
		authInfo.user = arr[0];
		if (arr.size() == 2)
			authInfo.password = arr[1];
	}
	else 
	{
		if (toolsString::contains(httpAuthorization, "Digest"))
		{
			authInfo.type = CLIENT_AUTHTYPE_DIGEST;
			
			dodoStringArray parts = tools::explode(httpAuthorization.substr(7), &trim, ",");
			
			dodoStringArray tuple;
			
			dodoStringArray::iterator i = parts.begin(), j = parts.end();
			for (;i!=j;++i)
			{
				tuple = tools::explode(*i, "=", 2);
				if (tuple.size() != 2)
					continue;
				
				dodoString &challengePart = tuple[0];
				
				if (toolsString::iequal(challengePart, "realm"))
					authInfo.realm = toolsString::trim(tuple[1], '"');
				else
				{
					if (toolsString::iequal(challengePart, "nonce"))
						authInfo.nonce = toolsString::trim(tuple[1], '"');
					else
					{
						if (toolsString::iequal(challengePart, "opaque"))
							authInfo.opaque = toolsString::trim(tuple[1], '"');
						else
						{
							if (toolsString::iequal(challengePart, "username"))
								authInfo.user = toolsString::trim(tuple[1], '"');
							else
							{
								if (toolsString::iequal(challengePart, "uri"))
									authInfo.uri = toolsString::trim(tuple[1], '"');
								else
								{
									if (toolsString::iequal(challengePart, "qop"))
										authInfo.qop = toolsString::trim(tuple[1], '"');
									else
									{
										if (toolsString::iequal(challengePart, "nc"))
											authInfo.nonceCount = toolsString::trim(tuple[1], '"');
										else
										{
											if (toolsString::iequal(challengePart, "cnonce"))
												authInfo.cnonce = toolsString::trim(tuple[1], '"');
											else
											{
												if (toolsString::iequal(challengePart, "response"))
													authInfo.response = toolsString::trim(tuple[1], '"');
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		else
			authInfo.type = CLIENT_AUTHTYPE_NONE;
	}
}

//-------------------------------------------------------------------

void 
client::requestAuthentification(const dodoString &realm,
			short type)
{
	returnCode = CLIENT_STATUSCODE_UNAUTHORIZED;
	
	if (type == CLIENT_AUTHTYPE_BASIC)
		HEADERS.insert(make_pair(CLIENT_RESPONSEHEADER_WWWAUTHENTICATE, dodoString("Basic realm=\"") + realm + "\""));
	else
		if (type == CLIENT_AUTHTYPE_DIGEST)
			HEADERS.insert(make_pair(CLIENT_RESPONSEHEADER_WWWAUTHENTICATE, dodoString("Digest realm=\"") + 
																					realm + 
																					"\", qop=\"auth\", nonce=\"" + 
																					tools::MD5Hex(tools::stringRandom(16)) + 
																					"\", opaque=\"" + 
																					tools::MD5Hex(tools::stringRandom(16)) + "\""));
}

//-------------------------------------------------------------------

__clientAuthInfo 
client::getAuthentificationInfo()
{
	__clientAuthInfo info = {authInfo.user, authInfo.type};

	return info;
}

//-------------------------------------------------------------------

bool 
client::checkAuthentification(const dodoString &user, 
								const dodoString &password)
{
	if (authInfo.type == CLIENT_AUTHTYPE_BASIC)
		return (toolsString::equal(user,authInfo.user) && toolsString::equal(password,authInfo.password));
	else
	{
		if (authInfo.type == CLIENT_AUTHTYPE_DIGEST)
		{

			unsigned char HA[16];			
			tools::MD5_CTX context;
						
			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)authInfo.user.c_str(), authInfo.user.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)authInfo.realm.c_str(), authInfo.realm.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)password.c_str(), password.size());
			tools::MD5Final(HA, &context);

			dodoString HA1 = tools::binToHex(dodoString((char *)&HA, 16));
			
			dodoString &methodForAuth = ENVIRONMENT[CLIENT_ENVIRONMENT_REQUESTMETHOD]; 
			
			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)methodForAuth.c_str(), methodForAuth.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)authInfo.uri.c_str(), authInfo.uri.size());
			tools::MD5Final(HA, &context);

			dodoString HA2 = tools::binToHex(dodoString((char *)&HA, 16));
			
			tools::MD5Init(&context);
			tools::MD5Update(&context, (unsigned char *)HA1.c_str(), HA1.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)authInfo.nonce.c_str(), authInfo.nonce.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)authInfo.nonceCount.c_str(), authInfo.nonceCount.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)authInfo.cnonce.c_str(), authInfo.cnonce.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)authInfo.qop.c_str(), authInfo.qop.size());
			tools::MD5Update(&context, (unsigned char *)":", 1);
			tools::MD5Update(&context, (unsigned char *)HA2.c_str(), HA2.size());
			tools::MD5Final(HA, &context);
			
			return (toolsString::equal(tools::binToHex(dodoString((char *)&HA, 16)), authInfo.response));
		}
		else
			return false;
	}
}

//-------------------------------------------------------------------

dodoString
client::getContent()
{
	return content;
}

//-------------------------------------------------------------------

void
client::clearContent()
{
	content.clear();
}

//-------------------------------------------------------------------

void
client::cleanTmp()
{
	dodoMap<dodoString, __clientFile>::iterator i(FILES.begin()), j(FILES.end());
	for (; i != j; ++i)
	{
		if (!postFilesInMem)
			unlink(i->second.tmp_name.c_str());
	}
}

//-------------------------------------------------------------------

void
client::detectMethod()
{
	if (toolsString::iequal(ENVIRONMENT[CLIENT_ENVIRONMENT_REQUESTMETHOD], "GET"))
		method = CLIENT_REQUESTMETHOD_GET;
	else
	{
		if (toolsString::iequal(ENVIRONMENT[CLIENT_ENVIRONMENT_REQUESTMETHOD], "POST") && ENVIRONMENT[CLIENT_ENVIRONMENT_REQUESTMETHOD].empty())
			method = CLIENT_REQUESTMETHOD_POST;
		else
			method = CLIENT_REQUESTMETHOD_GET_POST;
	}
}

//-------------------------------------------------------------------

int
client::getMethod() const
{
	return method;
}

//-------------------------------------------------------------------

void
client::make(dodoStringMap &val,
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
client::makeEnv()
{
	char *env;

	for (int i = 0; i < CLIENT_ENVIRONMENTSTATEMENTS; ++i)
	{
#ifdef FASTCGI_EXT
		if (clientFastSet)
			env = ((fastClientExchange *)cgiIO)->getenv(environmentStatements[i]);
		else
#endif
		env = getenv(environmentStatements[i]);

		ENVIRONMENT[i] = env == NULL ? "NULL" : env;
	}
	
	ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTTYPE] = tools::explode(ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTTYPE], ";", 2).front();
}

//-------------------------------------------------------------------

void
client::initHeaders(dodoMap<short, dodoString> &headers)
{
	if (headers.size() > 0)
	{
		HEADERS.insert(headers.begin(), headers.end());
	}
	else
	{
		HEADERS.insert(make_pair(CLIENT_RESPONSEHEADER_CONTENTTYPE, dodoString("text/html")));
		HEADERS.insert(make_pair(CLIENT_RESPONSEHEADER_XPOWEREDBY, dodoString(PACKAGE_NAME "/" PACKAGE_VERSION)));
	}
}

//-------------------------------------------------------------------

void 
client::setResponseStatus(short code)
{
	if (code <= CLIENT_STATUSCODE_HTTPVERSIONNOTSUPPORTED)
		returnCode = code;
	else
		throw baseEx(ERRMODULE_CGICLIENT, CLIENTEX_SETRESPONSESTATUS, ERR_LIBDODO, CLIENTEX_WRONGSTATUSCODE, CLIENTEX_WRONGSTATUSCODE_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
client::printHeaders() const
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
		dodoList<__clientCookie>::const_iterator i(cookies.begin()), j(cookies.end());
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
client::makeContent()
{
	unsigned long inSize = toolsString::stringToUL(ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTLENGTH]);

	if (inSize <= 0)
		return ;

	cgiIO->inSize = inSize;
	
	cgiIO->readString(content);
}

//-------------------------------------------------------------------

void
client::makePost()
{
	if (content.size() == 0)
		return ;

	if (!toolsString::iequal(ENVIRONMENT[CLIENT_ENVIRONMENT_REQUESTMETHOD], "POST"))
		return ;

	if (toolsString::iequal(ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTTYPE], "application/x-www-form-urlencoded"))
	{
		make(POST, content);
		
		content.clear();
	}
	else
	{
		if (toolsString::contains(ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTTYPE], "multipart/form-data"))
		{
			if (toolsString::iequal(ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTTRANSFERENCODING], "base64"))
				content = tools::decodeBase64(content);
	
			unsigned int temp0;
			temp0 = ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTTYPE].find("boundary=");
			dodoStringArray postPartd = tools::explode(content, "--" + ENVIRONMENT[CLIENT_ENVIRONMENT_CONTENTTYPE].substr(temp0 + 9));
	
			dodoStringArray::iterator i(postPartd.begin() + 1), j(postPartd.end());
	
			unsigned int temp1;
			char *ptr;
			int fd;
			unsigned short pathLength = postFilesTmpDir.size() + 18;
			FILE *fp;

			for (; i != j; ++i)
			{
				if (toolsString::iequal(i->substr(0, 2), "--"))
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
		
						__clientFile file;
		
						temp0 = i->find("filename=\"", temp1);
						temp0 += 10;
						temp1 = i->find("\"", temp0);
						if (temp0 == temp1)
							continue;
		
						file.name = i->substr(temp0, temp1 - temp0);
		
						temp0 = toolsString::find(*i, "Content-Type: ", temp1, true);
						temp0 += 14;
						temp1 = i->find("\n", temp0);
						file.type = tools::explode(i->substr(temp0, temp1 - temp0), ";")[0];
						temp1 += 3;
		
						file.size = i->size() - temp1 - 2;
		
						if (postFilesInMem)
							file.data.assign(i->c_str() + temp1, file.size);
						else
						{
							file.error = CLIENT_POSTFILEERR_NONE;
		
							ptr = new char[pathLength];
							strncpy(ptr, dodoString(postFilesTmpDir + FILE_DELIM + dodoString("dodo_post_XXXXXX")).c_str(), pathLength);
							fd = mkstemp(ptr);
							if (fd == -1)
							{
								delete [] ptr;
		
								file.error = CLIENT_POSTFILEERR_BAD_FILE_NAME;
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
		
										file.error = CLIENT_POSTFILEERR_ACCESS_DENY;
		
										break;
		
									case ENAMETOOLONG:
									case ENOTDIR:
		
										file.error = CLIENT_POSTFILEERR_BAD_FILE_NAME;
		
										break;
		
									case ENOMEM:
		
										file.error = CLIENT_POSTFILEERR_NO_SPACE;
		
										break;
									
									default:
										
										throw baseEx(ERRMODULE_CGICLIENT, CLIENTEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
								}
							}
							else
							{
								if (fwrite(i->c_str() + temp1, file.size, 1, fp) == 0)
								{
									if (errno == ENOMEM)
										file.error = CLIENT_POSTFILEERR_NO_SPACE;
									else
									{
										if (fclose(fp) != 0)
											throw baseEx(ERRMODULE_CGICLIENT, CLIENTEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
									
										throw baseEx(ERRMODULE_CGICLIENT, CLIENTEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
									}
								}
		
								if (fclose(fp) != 0)
									throw baseEx(ERRMODULE_CGICLIENT, CLIENTEX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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

const dodo::dodoStringMap &
client::operator[](short method)
{
	if (method == CLIENT_REQUESTMETHOD_POST)
		return POST;
	else
		return GET;
}

//-------------------------------------------------------------------

dodoString
client::request(const dodoString &varName,
			 short first)
{
	dodoString met0 = GET[varName];
	dodoString met1 = POST[varName];

	if (first == CLIENT_REQUESTMETHOD_GET)
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
client::setCookie(const dodoString &name,
			   const dodoString &value,
			   const dodoString &expires,
			   const dodoString &path,
			   const dodoString &domain,
			   bool secure)
{
	__clientCookie temp(secure);
	temp.name = name;
	temp.value = tools::encodeUrl(value);
	temp.expires = expires;
	temp.path = path;
	temp.domain = domain;
	cookies.push_back(temp);
}

//-------------------------------------------------------------------

void
client::setCookie(const __clientCookie &cookie)
{
	cookies.push_back(cookie);
}

//-------------------------------------------------------------------

dodoString 
client::trim(const dodoString &data)
{
	return toolsString::trim(data, ' ');
}

//-------------------------------------------------------------------

