/***************************************************************************
 *            cgiServer.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/cgiServer.h>

using namespace dodo::cgi;

const char *server::environmentStatements[] = { "REQUEST_METHOD",
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
												"SCRIPT_NAME",               };

//-------------------------------------------------------------------

const dodoString server::responseHeaderStatements[] = { "Accept-Ranges",
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
														"X-Powered-By",        };

const dodoString server::responseStatusStatements[] = { "Status: 100 Continue\r\n",
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
														"Status: 505 HTTP Version Not Supported\r\n",      };

//-------------------------------------------------------------------

__serverFile::__serverFile() : size(0),
							   error(SERVER_POSTFILEERR_NO_FILE)
{
}

//-------------------------------------------------------------------

__serverCookie::__serverCookie(const dodoString &a_name,
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

__serverCookie::__serverCookie() : secure(false)
{
}

//-------------------------------------------------------------------

__serverCookie::__serverCookie(bool a_secure) : secure(a_secure)
{
}

//-------------------------------------------------------------------

server::server(server &ct)
{
}

//-------------------------------------------------------------------

server::server(dodoMap<short, dodoString> &headers,
			   bool silent,
			   bool a_autocleanFiles,
			   bool a_postFilesInMem,
			   dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
											   postFilesTmpDir(a_postFilesTmpDir),
											   autocleanFiles(a_autocleanFiles),
											   headersPrinted(false),
											   returnCode(SERVER_STATUSCODE_OK)
#ifdef FASTCGI_EXT
											   ,
											   serverFastSet(false)
#endif

{
	authInfo.type = SERVER_AUTHTYPE_NONE;

	cgiIO = new dodo::io::stdio;

	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();

	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[SERVER_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[SERVER_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

server::server(bool silent,
			   bool a_autocleanFiles,
			   bool a_postFilesInMem,
			   dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
											   postFilesTmpDir(a_postFilesTmpDir),
											   autocleanFiles(a_autocleanFiles),
											   headersPrinted(false),
											   returnCode(SERVER_STATUSCODE_OK)
#ifdef FASTCGI_EXT
											   ,
											   serverFastSet(false)
#endif

{
	authInfo.type = SERVER_AUTHTYPE_NONE;

	cgiIO = new dodo::io::stdio;

	dodoMap<short, dodoString> headers;
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();

	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[SERVER_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[SERVER_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

#ifdef FASTCGI_EXT

server::server(fast::exchange    *a_cf,
			   bool silent,
			   bool a_autocleanFiles,
			   bool a_postFilesInMem,
			   dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
											   postFilesTmpDir(a_postFilesTmpDir),
											   serverFastSet(true),
											   cgiIO(a_cf),
											   autocleanFiles(a_autocleanFiles),
											   headersPrinted(false),
											   returnCode(SERVER_STATUSCODE_OK)

{
	authInfo.type = SERVER_AUTHTYPE_NONE;

	dodoMap<short, dodoString> headers;
	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();

	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[SERVER_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[SERVER_ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

server::server(fast::exchange    *a_cf,
			   dodoMap<short, dodoString> &headers,
			   bool silent,
			   bool a_autocleanFiles,
			   bool a_postFilesInMem,
			   dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
											   postFilesTmpDir(a_postFilesTmpDir),
											   serverFastSet(true),
											   cgiIO(a_cf),
											   autocleanFiles(a_autocleanFiles),
											   headersPrinted(false),
											   returnCode(SERVER_STATUSCODE_OK)

{
	authInfo.type = SERVER_AUTHTYPE_NONE;

	initHeaders(headers);

	if (!silent)
		printHeaders();

	makeEnv();

	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	make(COOKIES, ENVIRONMENT[SERVER_ENVIRONMENT_HTTPCOOKIE], "; ");
	make(GET, ENVIRONMENT[SERVER_ENVIRONMENT_QUERYSTRING]);
}


#endif

//-------------------------------------------------------------------

server::~server()
{
	printHeaders();

	if (autocleanFiles)
		cleanTmp();

#ifdef FASTCGI_EXT
	if (!serverFastSet)
		delete cgiIO;
#endif
}

//-------------------------------------------------------------------

void
server::flush()
{
	cgiIO->flush();
}

//-------------------------------------------------------------------

void
server::printStream(const dodoString &buf)
{
	printHeaders();

	cgiIO->writeStreamString(buf);
}

//-------------------------------------------------------------------

void
server::print(const dodoString &buf)
{
	printHeaders();

	cgiIO->outSize = buf.size();
	cgiIO->writeString(buf);
}

//-------------------------------------------------------------------

void
server::makeAuth()
{
	dodoString &httpAuthorization = ENVIRONMENT[SERVER_ENVIRONMENT_HTTPAUTHORIZATION];

	if (tools::string::contains(httpAuthorization, "Basic"))
	{
		dodoStringArray arr = tools::misc::explode(tools::misc::decodeBase64(tools::string::trim(httpAuthorization.substr(6), ' ')), ":", 2);

		authInfo.type = SERVER_AUTHTYPE_BASIC;
		authInfo.user = arr[0];
		if (arr.size() == 2)
			authInfo.password = arr[1];
	}
	else
	{
		if (tools::string::contains(httpAuthorization, "Digest"))
		{
			authInfo.type = SERVER_AUTHTYPE_DIGEST;

			dodoStringArray parts = tools::misc::explode(httpAuthorization.substr(7), &trim, ",");

			dodoStringArray tuple;

			dodoStringArray::iterator i = parts.begin(), j = parts.end();
			for (; i != j; ++i)
			{
				tuple = tools::misc::explode(*i, "=", 2);
				if (tuple.size() != 2)
					continue;

				dodoString &challengePart = tuple[0];

				if (tools::string::iequal(challengePart, "realm"))
					authInfo.realm = tools::string::trim(tuple[1], '"');
				else
				{
					if (tools::string::iequal(challengePart, "nonce"))
						authInfo.nonce = tools::string::trim(tuple[1], '"');
					else
					{
						if (tools::string::iequal(challengePart, "opaque"))
							authInfo.opaque = tools::string::trim(tuple[1], '"');
						else
						{
							if (tools::string::iequal(challengePart, "username"))
								authInfo.user = tools::string::trim(tuple[1], '"');
							else
							{
								if (tools::string::iequal(challengePart, "uri"))
									authInfo.uri = tools::string::trim(tuple[1], '"');
								else
								{
									if (tools::string::iequal(challengePart, "qop"))
										authInfo.qop = tools::string::trim(tuple[1], '"');
									else
									{
										if (tools::string::iequal(challengePart, "nc"))
											authInfo.nonceCount = tools::string::trim(tuple[1], '"');
										else
										{
											if (tools::string::iequal(challengePart, "cnonce"))
												authInfo.cnonce = tools::string::trim(tuple[1], '"');
											else
											{
												if (tools::string::iequal(challengePart, "response"))
													authInfo.response = tools::string::trim(tuple[1], '"');
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
			authInfo.type = SERVER_AUTHTYPE_NONE;
	}
}

//-------------------------------------------------------------------

void
server::requestAuthentication(const dodoString &realm,
							  short type)
{
	returnCode = SERVER_STATUSCODE_UNAUTHORIZED;

	if (type == SERVER_AUTHTYPE_BASIC)
		HEADERS.insert(make_pair(SERVER_RESPONSEHEADER_WWWAUTHENTICATE, dodoString("Basic realm=\"") + realm + "\""));
	else
	if (type == SERVER_AUTHTYPE_DIGEST)
		HEADERS.insert(make_pair(SERVER_RESPONSEHEADER_WWWAUTHENTICATE, dodoString("Digest realm=\"") +
								 realm +
								 "\", qop=\"auth\", nonce=\"" +
								 tools::misc::MD5Hex(tools::misc::stringRandom(16)) +
								 "\", opaque=\"" +
								 tools::misc::MD5Hex(tools::misc::stringRandom(16)) + "\""));
}

//-------------------------------------------------------------------

__serverAuthInfo
server::getAuthenticationInfo()
{
	__serverAuthInfo info = { authInfo.user, authInfo.type };

	return info;
}

//-------------------------------------------------------------------

bool
server::checkAuthentication(const dodoString &user,
							const dodoString &password)
{
	if (authInfo.type == SERVER_AUTHTYPE_BASIC)
		return(tools::string::equal(user, authInfo.user) && tools::string::equal(password, authInfo.password));
	else
	{
		if (authInfo.type == SERVER_AUTHTYPE_DIGEST)
		{

			unsigned char HA[16];
			tools::misc::MD5_CTX context;

			tools::misc::MD5Init(&context);
			tools::misc::MD5Update(&context, (unsigned char *)authInfo.user.c_str(), authInfo.user.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)authInfo.realm.c_str(), authInfo.realm.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)password.c_str(), password.size());
			tools::misc::MD5Final(HA, &context);

			dodoString HA1 = tools::misc::binToHex(dodoString((char *)&HA, 16));

			dodoString &methodForAuth = ENVIRONMENT[SERVER_ENVIRONMENT_REQUESTMETHOD];

			tools::misc::MD5Init(&context);
			tools::misc::MD5Update(&context, (unsigned char *)methodForAuth.c_str(), methodForAuth.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)authInfo.uri.c_str(), authInfo.uri.size());
			tools::misc::MD5Final(HA, &context);

			dodoString HA2 = tools::misc::binToHex(dodoString((char *)&HA, 16));

			tools::misc::MD5Init(&context);
			tools::misc::MD5Update(&context, (unsigned char *)HA1.c_str(), HA1.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)authInfo.nonce.c_str(), authInfo.nonce.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)authInfo.nonceCount.c_str(), authInfo.nonceCount.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)authInfo.cnonce.c_str(), authInfo.cnonce.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)authInfo.qop.c_str(), authInfo.qop.size());
			tools::misc::MD5Update(&context, (unsigned char *)":", 1);
			tools::misc::MD5Update(&context, (unsigned char *)HA2.c_str(), HA2.size());
			tools::misc::MD5Final(HA, &context);

			return(tools::string::equal(tools::misc::binToHex(dodoString((char *)&HA, 16)), authInfo.response));
		}
		else
			return false;
	}
}

//-------------------------------------------------------------------

dodoString
server::getContent()
{
	return content;
}

//-------------------------------------------------------------------

void
server::clearContent()
{
	content.clear();
}

//-------------------------------------------------------------------

void
server::cleanTmp()
{
	dodoMap<dodoString, __serverFile>::iterator i(FILES.begin()), j(FILES.end());
	for (; i != j; ++i)
	{
		if (!postFilesInMem)
			unlink(i->second.tmp_name.c_str());
	}
}

//-------------------------------------------------------------------

void
server::detectMethod()
{
	if (tools::string::iequal(ENVIRONMENT[SERVER_ENVIRONMENT_REQUESTMETHOD], "GET"))
		method = SERVER_REQUESTMETHOD_GET;
	else
	{
		if (tools::string::iequal(ENVIRONMENT[SERVER_ENVIRONMENT_REQUESTMETHOD], "POST") && ENVIRONMENT[SERVER_ENVIRONMENT_REQUESTMETHOD].empty())
			method = SERVER_REQUESTMETHOD_POST;
		else
			method = SERVER_REQUESTMETHOD_GET_POST;
	}
}

//-------------------------------------------------------------------

int
server::getMethod() const
{
	return method;
}

//-------------------------------------------------------------------

void
server::make(dodoStringMap &val,
			 const dodoString &string,
			 const char       *delim)
{
	dodoStringArray getPair = tools::misc::explode(tools::misc::decodeUrl(string), delim);

	dodoStringArray::iterator l(getPair.begin()), m(getPair.end());

	dodoStringArray temp;

	for (; l != m; ++l)
	{
		temp = tools::misc::explode(*l, "=");
		if (temp.size() > 1)
			val.insert(make_pair(temp[0], temp[1]));
	}
}

//-------------------------------------------------------------------

void
server::makeEnv()
{
	char *env;

	for (int i = 0; i < SERVER_ENVIRONMENTSTATEMENTS; ++i)
	{
#ifdef FASTCGI_EXT
		if (serverFastSet)
			env = ((fast::exchange *)cgiIO)->getenv(environmentStatements[i]);
		else
#endif
		env = getenv(environmentStatements[i]);

		ENVIRONMENT[i] = env == NULL ? "NULL" : env;
	}

	ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTTYPE] = tools::misc::explode(ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTTYPE], ";", 2).front();
}

//-------------------------------------------------------------------

void
server::initHeaders(dodoMap<short, dodoString> &headers)
{
	if (headers.size() > 0)
	{
		HEADERS.insert(headers.begin(), headers.end());
	}
	else
	{
		HEADERS.insert(make_pair(SERVER_RESPONSEHEADER_CONTENTTYPE, dodoString("text/html")));
		HEADERS.insert(make_pair(SERVER_RESPONSEHEADER_XPOWEREDBY, dodoString(PACKAGE_NAME "/" PACKAGE_VERSION)));
	}
}

//-------------------------------------------------------------------

void
server::setResponseStatus(short code)
{
	if (code <= SERVER_STATUSCODE_HTTPVERSIONNOTSUPPORTED)
		returnCode = code;
	else
		throw baseEx(ERRMODULE_CGISERVER, SERVEREX_SETRESPONSESTATUS, ERR_LIBDODO, SERVEREX_WRONGSTATUSCODE, CGISERVEREX_WRONGSTATUSCODE_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
server::printHeaders() const
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
		dodoList<__serverCookie>::const_iterator i(cookies.begin()), j(cookies.end());
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
server::makeContent()
{
	unsigned long inSize = tools::string::stringToUL(ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTLENGTH]);

	if (inSize <= 0)
		return ;

	cgiIO->inSize = inSize;

	cgiIO->readString(content);
}

//-------------------------------------------------------------------

void
server::makePost()
{
	if (content.size() == 0)
		return ;

	if (!tools::string::iequal(ENVIRONMENT[SERVER_ENVIRONMENT_REQUESTMETHOD], "POST"))
		return ;

	if (tools::string::iequal(ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTTYPE], "application/x-www-form-urlencoded"))
	{
		make(POST, content);

		content.clear();
	}
	else
	{
		if (tools::string::contains(ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTTYPE], "multipart/form-data"))
		{
			if (tools::string::iequal(ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTTRANSFERENCODING], "base64"))
				content = tools::misc::decodeBase64(content);

			unsigned int temp0;
			temp0 = ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTTYPE].find("boundary=");
			dodoStringArray postPartd = tools::misc::explode(content, "--" + ENVIRONMENT[SERVER_ENVIRONMENT_CONTENTTYPE].substr(temp0 + 9));

			dodoStringArray::iterator i(postPartd.begin() + 1), j(postPartd.end());

			unsigned int temp1;
			char *ptr;
			int fd;
			unsigned short pathLength = postFilesTmpDir.size() + 18;
			FILE *fp;

			for (; i != j; ++i)
			{
				if (tools::string::iequal(i->substr(0, 2), "--"))
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

						__serverFile file;

						temp0 = i->find("filename=\"", temp1);
						temp0 += 10;
						temp1 = i->find("\"", temp0);
						if (temp0 == temp1)
							continue;

						file.name = i->substr(temp0, temp1 - temp0);

						temp0 = tools::string::find(*i, "Content-Type: ", temp1, true);
						temp0 += 14;
						temp1 = i->find("\n", temp0);
						file.type = tools::misc::explode(i->substr(temp0, temp1 - temp0), ";")[0];
						temp1 += 3;

						file.size = i->size() - temp1 - 2;

						if (postFilesInMem)
							file.data.assign(i->c_str() + temp1, file.size);
						else
						{
							file.error = SERVER_POSTFILEERR_NONE;

							ptr = new char[pathLength];
							strncpy(ptr, dodoString(postFilesTmpDir + FILE_DELIM + dodoString("dodo_post_XXXXXX")).c_str(), pathLength);
							fd = mkstemp(ptr);
							if (fd == -1)
							{
								delete [] ptr;

								file.error = SERVER_POSTFILEERR_BAD_FILE_NAME;
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

										file.error = SERVER_POSTFILEERR_ACCESS_DENY;

										break;

									case ENAMETOOLONG:
									case ENOTDIR:

										file.error = SERVER_POSTFILEERR_BAD_FILE_NAME;

										break;

									case ENOMEM:

										file.error = SERVER_POSTFILEERR_NO_SPACE;

										break;

									default:

										throw baseEx(ERRMODULE_CGISERVER, SERVEREX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
								}
							}
							else
							{
								if (fwrite(i->c_str() + temp1, file.size, 1, fp) == 0)
								{
									if (errno == ENOMEM)
										file.error = SERVER_POSTFILEERR_NO_SPACE;
									else
									{
										if (fclose(fp) != 0)
											throw baseEx(ERRMODULE_CGISERVER, SERVEREX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

										throw baseEx(ERRMODULE_CGISERVER, SERVEREX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
									}
								}

								if (fclose(fp) != 0)
									throw baseEx(ERRMODULE_CGISERVER, SERVEREX_MAKEPOST, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
server::operator[](short method)
{
	if (method == SERVER_REQUESTMETHOD_POST)
		return POST;
	else
		return GET;
}

//-------------------------------------------------------------------

dodoString
server::request(const dodoString &varName,
				short first)
{
	dodoString met0 = GET[varName];
	dodoString met1 = POST[varName];

	if (first == SERVER_REQUESTMETHOD_GET)
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
server::setCookie(const dodoString &name,
				  const dodoString &value,
				  const dodoString &expires,
				  const dodoString &path,
				  const dodoString &domain,
				  bool secure)
{
	__serverCookie temp(secure);
	temp.name = name;
	temp.value = tools::misc::encodeUrl(value);
	temp.expires = expires;
	temp.path = path;
	temp.domain = domain;
	cookies.push_back(temp);
}

//-------------------------------------------------------------------

void
server::setCookie(const __serverCookie &cookie)
{
	cookies.push_back(cookie);
}

//-------------------------------------------------------------------

dodoString
server::trim(const dodoString &data)
{
	return tools::string::trim(data, ' ');
}

//-------------------------------------------------------------------

