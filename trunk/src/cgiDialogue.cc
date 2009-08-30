/***************************************************************************
 *            cgiDialogue.cc
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

#include <libdodo/directives.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include <libdodo/cgiDialogue.h>
#include <libdodo/types.h>
#include <libdodo/cgi.h>
#include <libdodo/cgiExchange.h>
#include <libdodo/cgiDialogueEx.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsCode.h>
#include <libdodo/toolsMisc.h>

using namespace dodo::cgi;

const char *dialogue::environmentStatements[] = {
	"REQUEST_METHOD",
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
	"DIALOGUE_SIGNATURE",
	"DIALOGUE_SOFTWARE",
	"DIALOGUE_NAME",
	"DIALOGUE_ADDR",
	"DIALOGUE_PORT",
	"DIALOGUE_ADMIN",
	"DIALOGUE_PROTOCOL",
	"SCRIPT_FILENAME",
	"SCRIPT_NAME",
};

//-------------------------------------------------------------------

const dodoString dialogue::responseHeaderStatements[] = {
	"Accept-Ranges",
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

const dodoString dialogue::responseStatusStatements[] = {
	"Status: 100 Continue\r\n",
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

dialogue::dialogue(dialogue &ct) : io(ct.io)
{
}

//-------------------------------------------------------------------

dialogue::dialogue(exchange   &a_cf,
				   bool       a_autocleanFiles,
				   bool       a_postFilesInMem,
				   dodoString a_postFilesTmpDir) : returnCode(STATUS_CODE_OK),
												   postFilesInMem(a_postFilesInMem),
												   autocleanFiles(a_autocleanFiles),
												   postFilesTmpDir(a_postFilesTmpDir),
												   io(a_cf),
												   headersPrinted(false)

{
	authInfo.type = AUTH_NONE;

	dodoMap<short, dodoString> headers;
	initHeaders(headers);

	makeEnv();

	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	makeKeyValue(COOKIES, ENVIRONMENT[ENVIRONMENT_HTTPCOOKIE], "; ");
	makeKeyValue(GET, ENVIRONMENT[ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

dialogue::dialogue(exchange &a_cf,
				   dodoMap<short, dodoString> &headers,
				   bool a_autocleanFiles,
				   bool a_postFilesInMem,
				   dodoString a_postFilesTmpDir) : returnCode(STATUS_CODE_OK),
												   postFilesInMem(a_postFilesInMem),
												   autocleanFiles(a_autocleanFiles),
												   postFilesTmpDir(a_postFilesTmpDir),
												   io(a_cf),
												   headersPrinted(false)

{
	authInfo.type = AUTH_NONE;

	initHeaders(headers);

	printHeaders();

	makeEnv();

	makeAuth();

	detectMethod();

	makeContent();
	makePost();

	makeKeyValue(COOKIES, ENVIRONMENT[ENVIRONMENT_HTTPCOOKIE], "; ");
	makeKeyValue(GET, ENVIRONMENT[ENVIRONMENT_QUERYSTRING]);
}

//-------------------------------------------------------------------

dialogue::~dialogue()
{
	printHeaders();

	if (autocleanFiles)
		cleanTmp();
}

//-------------------------------------------------------------------

dialogue::operator exchange
*()
{
	printHeaders();

	return &io;
}

//-------------------------------------------------------------------

void
dialogue::flush() const
{
	io.flush();
}

//-------------------------------------------------------------------

void
dialogue::printString(const dodoString &buf)
{
	printHeaders();

	io.writeString(buf);
}

//-------------------------------------------------------------------

dodoString
dialogue::charset()
{
	unsigned long temp;

	dodoStringArray::iterator b = contenTypeExtensions.begin(), e = contenTypeExtensions.end();
	for (; b != e; ++b) {
		temp = tools::string::find(*b, "charset=", true);
		if (temp == dodoString::npos)
			continue;

		return dodoString(b->data() + temp + 8, b->size() - temp - 8);
	}

	return __dodostring__;
}

void
dialogue::print(const dodoString &buf)
{
	printHeaders();

	io.outSize = buf.size();
	io.write(buf);
}

//-------------------------------------------------------------------

void
dialogue::makeAuth()
{
	dodoString &httpAuthorization = ENVIRONMENT[ENVIRONMENT_HTTPAUTHORIZATION];

	if (tools::string::contains(httpAuthorization, "Basic")) {
		dodoStringArray arr = tools::misc::split(tools::code::decodeBase64(tools::string::trim(dodoString(httpAuthorization.data() + 6, httpAuthorization.size() - 6), ' ')), ":", 2);

		authInfo.type = AUTH_BASIC;
		authInfo.user = arr[0];
		if (arr.size() == 2)
			authInfo.password = arr[1];
	} else {
		if (tools::string::contains(httpAuthorization, "Digest")) {
			authInfo.type = AUTH_DIGEST;

			dodoStringArray parts = tools::misc::split(dodoString(httpAuthorization.data() + 7, httpAuthorization.size() - 7), ",");

			dodoStringArray tuple;

			dodoStringArray::iterator i = parts.begin(), j = parts.end();
			for (; i != j; ++i) {
				*i = tools::string::trim(*i, ' ');
				tuple = tools::misc::split(*i, "=", 2);
				if (tuple.size() != 2)
					continue;

				dodoString &challengePart = tuple[0];

				if (tools::string::iequal(challengePart, "realm"))
					authInfo.realm = tools::string::trim(tuple[1], '"');
				else {
					if (tools::string::iequal(challengePart, "nonce"))
						authInfo.nonce = tools::string::trim(tuple[1], '"');
					else {
						if (tools::string::iequal(challengePart, "opaque"))
							authInfo.opaque = tools::string::trim(tuple[1], '"');
						else {
							if (tools::string::iequal(challengePart, "username"))
								authInfo.user = tools::string::trim(tuple[1], '"');
							else {
								if (tools::string::iequal(challengePart, "uri"))
									authInfo.uri = tools::string::trim(tuple[1], '"');
								else {
									if (tools::string::iequal(challengePart, "qop"))
										authInfo.qop = tools::string::trim(tuple[1], '"');
									else {
										if (tools::string::iequal(challengePart, "nc"))
											authInfo.nonceCount = tools::string::trim(tuple[1], '"');
										else {
											if (tools::string::iequal(challengePart, "cnonce"))
												authInfo.cnonce = tools::string::trim(tuple[1], '"');
											else if (tools::string::iequal(challengePart, "response"))
												authInfo.response = tools::string::trim(tuple[1], '"');

										}
									}
								}
							}
						}
					}
				}
			}
		} else
			authInfo.type = AUTH_NONE;
	}
}

//-------------------------------------------------------------------

void
dialogue::requestAuthentication(const dodoString &realm,
								short            type)
{
	returnCode = STATUS_CODE_UNAUTHORIZED;

	if (type == AUTH_BASIC)
		HEADERS.insert(make_pair(RESPONSE_HEADER_WWWAUTHENTICATE, dodoString("Basic realm=\"") + realm + "\""));
	else if (type == AUTH_DIGEST) {
		HEADERS.insert(make_pair(RESPONSE_HEADER_WWWAUTHENTICATE, dodoString("Digest realm=\"") +
								 realm +
								 "\", qop=\"auth\", nonce=\"" +
								 tools::code::MD5Hex(tools::misc::stringRandom(16)) +
								 "\", opaque=\"" +
								 tools::code::MD5Hex(tools::misc::stringRandom(16)) + "\""));
	}
}

//-------------------------------------------------------------------

dodo::cgi::__auth__
dialogue::authenticationResponse()
{
	cgi::__auth__ info = {
		authInfo.user, authInfo.type
	};

	return info;
}

//-------------------------------------------------------------------

bool
dialogue::isAuthenticated(const dodoString &user,
						  const dodoString &password)
{
	if (authInfo.type == AUTH_BASIC)
		return (tools::string::equal(user, authInfo.user) && tools::string::equal(password, authInfo.password));
	else {
		if (authInfo.type == AUTH_DIGEST) {
			unsigned char HA[16];
			tools::code::__MD5Context__ context;

			tools::code::MD5Init(&context);
			tools::code::MD5Update(&context, (unsigned char *)authInfo.user.data(), authInfo.user.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)authInfo.realm.data(), authInfo.realm.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)password.data(), password.size());
			tools::code::MD5Final(HA, &context);

			dodoString HA1 = tools::code::binToHex(dodoString((char *)&HA, 16));

			dodoString &methodForAuth = ENVIRONMENT[ENVIRONMENT_REQUESTMETHOD];

			tools::code::MD5Init(&context);
			tools::code::MD5Update(&context, (unsigned char *)methodForAuth.data(), methodForAuth.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)authInfo.uri.data(), authInfo.uri.size());
			tools::code::MD5Final(HA, &context);

			dodoString HA2 = tools::code::binToHex(dodoString((char *)&HA, 16));

			tools::code::MD5Init(&context);
			tools::code::MD5Update(&context, (unsigned char *)HA1.data(), HA1.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)authInfo.nonce.data(), authInfo.nonce.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)authInfo.nonceCount.data(), authInfo.nonceCount.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)authInfo.cnonce.data(), authInfo.cnonce.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)authInfo.qop.data(), authInfo.qop.size());
			tools::code::MD5Update(&context, (unsigned char *)":", 1);
			tools::code::MD5Update(&context, (unsigned char *)HA2.data(), HA2.size());
			tools::code::MD5Final(HA, &context);

			return (tools::string::equal(tools::code::binToHex(dodoString((char *)&HA, 16)), authInfo.response));
		} else
			return false;
	}
}

//-------------------------------------------------------------------

void
dialogue::cleanTmp()
{
	dodoMap<dodoString, file>::iterator i(FILES.begin()), j(FILES.end());
	for (; i != j; ++i)
		if (!postFilesInMem)
			unlink(i->second.path.data());
}

//-------------------------------------------------------------------

void
dialogue::detectMethod()
{
	if (tools::string::iequal(ENVIRONMENT[ENVIRONMENT_REQUESTMETHOD], "GET"))
		requestMethod = REQUEST_METHOD_GET;
	else {
		if (tools::string::iequal(ENVIRONMENT[ENVIRONMENT_REQUESTMETHOD], "POST") && ENVIRONMENT[ENVIRONMENT_REQUESTMETHOD].empty())
			requestMethod = REQUEST_METHOD_POST;
		else
			requestMethod = REQUEST_METHOD_GET_POST;
	}
}

//-------------------------------------------------------------------

int
dialogue::method() const
{
	return requestMethod;
}

//-------------------------------------------------------------------

void
dialogue::makeKeyValue(dodoStringMap    &val,
					   const dodoString &string,
					   const char       *delim)
{
	dodoStringArray getPair = tools::misc::split(tools::code::decodeUrl(string), delim);

	dodoStringArray::iterator l(getPair.begin()), m(getPair.end());

	dodoStringArray temp;

	for (; l != m; ++l) {
		temp = tools::misc::split(*l, "=");
		if (temp.size() > 1)
			val.insert(make_pair(temp[0], temp[1]));
	}
}

//-------------------------------------------------------------------

void
dialogue::makeEnv()
{
	char *env;

	for (int i = 0; i < ENVIRONMENT_ENUMSIZE; ++i) {
		env = io.getenv(environmentStatements[i]);

		ENVIRONMENT[i] = env == NULL ? "NULL" : env;
	}

	dodoStringArray contentTypeParts = tools::misc::split(ENVIRONMENT[ENVIRONMENT_CONTENTTYPE], ";");
	dodoStringArray::iterator i = contentTypeParts.begin(), j = contentTypeParts.end();
	for (; i != j; ++i)
		*i = tools::string::trim(*i, ' ');

	unsigned long size = contentTypeParts.size();
	if (size > 0) {
		dodoStringArray::iterator first = contentTypeParts.begin();

		ENVIRONMENT[ENVIRONMENT_CONTENTTYPE] = *first;

		if (size > 1)
			contenTypeExtensions.insert(contenTypeExtensions.begin(), first + 1, contentTypeParts.end());
	} else {
		contentTypeParts = tools::misc::split(ENVIRONMENT[ENVIRONMENT_CONTENTTYPE], ",");
		i = contentTypeParts.begin();
		j = contentTypeParts.end();
		for (; i != j; ++i)
			*i = tools::string::trim(*i, ' ');

		if (size > 0) {
			dodoStringArray::iterator first = contentTypeParts.begin();

			ENVIRONMENT[ENVIRONMENT_CONTENTTYPE] = *first;

			if (size > 1)
				contenTypeExtensions.insert(contenTypeExtensions.begin(), first + 1, contentTypeParts.end());
		}
	}
}

//-------------------------------------------------------------------

void
dialogue::initHeaders(dodoMap<short, dodoString> &headers)
{
	if (headers.size() > 0)
		HEADERS.insert(headers.begin(), headers.end());
	else {
		HEADERS.insert(make_pair(RESPONSE_HEADER_CONTENTTYPE, dodoString("text/html")));
		HEADERS.insert(make_pair(RESPONSE_HEADER_XPOWEREDBY, dodoString(PACKAGE_NAME "/" PACKAGE_VERSION)));
	}
}

//-------------------------------------------------------------------

void
dialogue::setResponseStatus(short code)
{
	if (code <= STATUS_CODE_HTTPVERSIONNOTSUPPORTED)
		returnCode = code;
	else
		throw exception::basic(exception::MODULE_CGIDIALOGUE, DIALOGUEEX_SETRESPONSESTATUS, exception::ERRNO_LIBDODO, DIALOGUEEX_WRONGSTATUSCODE, CGIDIALOGUEEX_WRONGSTATUSCODE_STR, __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
dialogue::printHeaders() const
{
	if (headersPrinted)
		return;

	headersPrinted = true;

	io.writeString(responseStatusStatements[returnCode]);

	dodoMap<short, dodoString>::const_iterator i(HEADERS.begin()), j(HEADERS.end());
	for (; i != j; ++i)
		io.writeString(responseHeaderStatements[i->first] + ": " + i->second + "\r\n");

	if (cookies.size() > 0) {
		dodoList<cookie>::const_iterator i(cookies.begin()), j(cookies.end());
		for (; i != j; ++i) {
			io.writeString("Set-Cookie: ");
			io.writeString(i->name + "=" + i->value + "; ");
			if (i->path.size() > 0)
				io.writeString("path=" + i->path + "; ");
			if (i->expires.size() > 0)
				io.writeString("expires=" + i->expires + "; ");
			if (i->domain.size() > 0)
				io.writeString("domain=" + i->domain + "; ");
			if (i->secure)
				io.writeString("secure");
			io.writeString("\r\n");
		}
	}

	io.writeString("\r\n");
	io.flush();
}

//-------------------------------------------------------------------

void
dialogue::makeContent()
{
	unsigned long inSize = tools::string::stringToUL(ENVIRONMENT[ENVIRONMENT_CONTENTLENGTH]);

	if (inSize <= 0)
		return ;

	io.inSize = inSize;

	content = io.read();
}

//-------------------------------------------------------------------

void
dialogue::makePost()
{
	if (content.size() == 0)
		return ;

	if (!tools::string::iequal(ENVIRONMENT[ENVIRONMENT_REQUESTMETHOD], "POST"))
		return ;

	if (tools::string::iequal(ENVIRONMENT[ENVIRONMENT_CONTENTTYPE], "application/x-www-form-urlencoded")) {
		makeKeyValue(POST, content);

		content.clear();
	} else {
		if (tools::string::iequal(ENVIRONMENT[ENVIRONMENT_CONTENTTYPE], "multipart/form-data")) {
			if (tools::string::iequal(ENVIRONMENT[ENVIRONMENT_CONTENTTRANSFERENCODING], "base64"))
				content = tools::code::decodeBase64(content);

			unsigned long temp0;
			dodoStringArray postParts;

			dodoStringArray::iterator b = contenTypeExtensions.begin(), e = contenTypeExtensions.end();
			for (; b != e; ++b) {
				temp0 = tools::string::find(*b, "boundary=", true);
				if (temp0 == dodoString::npos)
					continue;

				dodoString delimiter = "--";
				delimiter.append(b->data() + temp0 + 9, b->size() - temp0 - 9);

				postParts = tools::misc::split(content, delimiter);
			}

			if (postParts.size() > 0) {
				dodoStringArray::iterator i(postParts.begin() + 1), j(postParts.end());

				unsigned long temp1;
				char *ptr;
				int fd;
				unsigned short pathLength = postFilesTmpDir.size() + 18;
				FILE *fp;
				const char *argument;

				for (; i != j; ++i) {
					if (i->size() == 2 && (*i)[0] == '-' && (*i)[1] == '-') ///< '--' in the last portion
						break;
					else {
						if (i->find("filename") != dodoString::npos) {
							if ((temp0 = i->find("name=\"")) == dodoString::npos)
								continue;

							temp0 += 6;

							if ((temp1 = i->find("\"", temp0)) == dodoString::npos)
								continue;

							dodoString post_name = dodoString(i->data() + temp0, temp1 - temp0);

							file file;

							temp0 = i->find("filename=\"", temp1);
							temp0 += 10;
							temp1 = i->find("\"", temp0);
							if (temp0 == temp1)
								continue;

							file.name = dodoString(i->data() + temp0, temp1 - temp0);

							temp0 = tools::string::find(*i, "Content-Type: ", temp1, true);
							temp0 += 14;
							temp1 = i->find("\n", temp0);
							file.mime = tools::misc::split(dodoString(i->data() + temp0, temp1 - temp0), ";")[0];

							unsigned long lIndex = file.mime.size() - 1;
							if (file.mime[lIndex] == '\r')
								file.mime.erase(lIndex);
							temp1 += 3;

							file.size = i->size() - temp1 - 2;

							if (postFilesInMem)
								file.data.assign(i->data() + temp1, file.size);
							else {
								file.error = FILE_ERROR_NONE;

								ptr = new char[pathLength];
								strncpy(ptr, dodoString(postFilesTmpDir + "/" + dodoString("dodo_post_XXXXXX")).data(), pathLength);
								fd = mkstemp(ptr);
								if (fd == -1) {
									delete [] ptr;

									file.error = FILE_ERROR_BAD_FILE_NAME;
									FILES.insert(make_pair(post_name, file));

									continue;
								}

								file.path = ptr;

								delete [] ptr;

								fp = fdopen(fd, "w+");
								if (fp == NULL) {
									switch (errno) {
										case EACCES:
										case EISDIR:

											file.error = FILE_ERROR_ACCESS_DENY;

											break;

										case ENAMETOOLONG:
										case ENOTDIR:

											file.error = FILE_ERROR_BAD_FILE_NAME;

											break;

										case ENOMEM:

											file.error = FILE_ERROR_NO_SPACE;

											break;

										default:

											throw exception::basic(exception::MODULE_CGIDIALOGUE, DIALOGUEEX_MAKEPOST, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
									}
								} else {
									if (fwrite(i->data() + temp1, file.size, 1, fp) == 0) {
										if (errno == ENOMEM)
											file.error = FILE_ERROR_NO_SPACE;
										else {
											if (fclose(fp) != 0)
												throw exception::basic(exception::MODULE_CGIDIALOGUE, DIALOGUEEX_MAKEPOST, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

											throw exception::basic(exception::MODULE_CGIDIALOGUE, DIALOGUEEX_MAKEPOST, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
										}
									}

									if (fclose(fp) != 0)
										throw exception::basic(exception::MODULE_CGIDIALOGUE, DIALOGUEEX_MAKEPOST, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
								}
							}

							FILES.insert(make_pair(post_name, file));
						} else {
							if ((temp0 = i->find("name=\"")) == dodoString::npos)
								continue;

							temp0 += 6;

							if ((temp1 = i->find("\"", temp0)) == dodoString::npos)
								continue;

							argument = i->data();

							POST.insert(make_pair(dodoString(argument + temp0, temp1 - temp0), dodoString(argument + temp1 + 5, i->size() - temp1 - 7)));
						}
					}
				}
			}

			content.clear();
		}
	}
}

//-------------------------------------------------------------------

const dodo::dodoStringMap &
dialogue::operator[](short method)
{
	if (method == REQUEST_METHOD_POST)
		return POST;
	else
		return GET;
}

//-------------------------------------------------------------------

dodoString
dialogue::operator[](const dodoString &name)
{
	dodoStringMap::iterator item = GET.find(name);
	if (item != GET.end())
		return item->second;

	item = POST.find(name);
	if (item != POST.end())
		return item->second;

	return __dodostring__;
}

//-------------------------------------------------------------------

void
dialogue::setCookie(const cookie &cookie)
{
	cookies.push_back(cookie);
}

//-------------------------------------------------------------------

