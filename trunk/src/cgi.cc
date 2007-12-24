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

__cgiFile::__cgiFile() : fp(NULL),
							   buf(NULL),
							   size(0)
{
}

//-------------------------------------------------------------------

__cookies::__cookies(const dodoString &a_name,
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

__cookies::__cookies(bool a_secure) : secure(a_secure)
{
}

//-------------------------------------------------------------------

cgi::cgi(cgi &ct)
{
}

//-------------------------------------------------------------------

cgi::cgi(bool silent,
		 dodoStringMap &a_headers,
		 bool a_autoclearContent,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir)
				#ifdef FCGI_EXT
										 ,
										 cgiFastSet(false)
				#endif

{
	fstd = new ioSTD;

	initHeaders(a_headers);

	if (!silent)
		printHeaders();

	makeEnv();

	detectMethod();

	makeContent();
	makePost();
	
	if (autoclearContent)
		content.clear();

	make(COOKIES, ENVIRONMENT["HTTP_COOKIE"], "; ");
	make(METHOD_GET, ENVIRONMENT["QUERY_STRING"]);
}

//-------------------------------------------------------------------

#ifdef FCGI_EXT

cgi::cgi(cgiFastIO    *a_cf,
		 bool silent,
		 dodoStringMap &a_headers,
		 bool a_autoclearContent,
		 bool a_postFilesInMem,
		 dodoString a_postFilesTmpDir) : postFilesInMem(a_postFilesInMem),
										 postFilesTmpDir(a_postFilesTmpDir),
										 cgiFastSet(true),
										 cf(a_cf)

{
	initHeaders(a_headers);

	if (!silent)
		printHeaders();

	makeEnv();

	detectMethod();

	makeContent();
	makePost();

	if (autoclearContent)
		content.clear();

	make(COOKIES, ENVIRONMENT["HTTP_COOKIE"], "; ");
	make(METHOD_GET, ENVIRONMENT["QUERY_STRING"]);
}

#endif

//-------------------------------------------------------------------

cgi::~cgi()
{
	cleanTmp();

	if (!cgiFastSet)
		delete fstd;
}

//-------------------------------------------------------------------

void
cgi::flush()
{
	fflush(stdout);
}

//-------------------------------------------------------------------

void
cgi::print(const dodoString &buf)
{
	fwrite(buf.c_str(), buf.size(), 1, stdout);
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
		if (i->second.fp != NULL)
			fclose(i->second.fp);

		if (postFilesInMem)
			free(i->second.buf);
		else
			unlink(i->second.tmp_name.c_str());
	}
}

//-------------------------------------------------------------------

void
cgi::detectMethod()
{
	if (stringTools::iequal(ENVIRONMENT["REQUEST_METHOD"], "GET"))
		method = CGI_REQUESTMETHOD_GET;
	else
	{
		if (stringTools::iequal(ENVIRONMENT["REQUEST_METHOD"], "POST") && ENVIRONMENT["REQUEST_METHOD"].empty())
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
	dodoStringArr getPair = tools::explode(tools::decodeURL(string), delim);

	dodoStringArr::iterator l(getPair.begin()), m(getPair.end());

	dodoStringArr temp;

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

	for (int i = 0; i < HTTP_ENV_SIZE; ++i)
	{
		#ifdef FCGI_EXT
		if (cgiFastSet)
			env = cf->getenv(HTTP_ENV[i].str);
		else
		#endif
		env = getenv(HTTP_ENV[i].str);

		ENVIRONMENT.insert(HTTP_ENV[i].str, env == NULL ? "NULL" : env);
	}
}

//-------------------------------------------------------------------

void
cgi::initHeaders(dodoStringMap &headers)
{
	HEADERS.insert("Content-type", "text/html");
	HEADERS.insert("X-Powered-By", PACKAGE_NAME "/" PACKAGE_VERSION);

	if (headers.size() > 0)
	{
		dodoStringMap::iterator i(headers.begin()), j(headers.end());
		for (; i != j; ++i)
			HEADERS.insert(i->first, i->second);
	}
}

//-------------------------------------------------------------------

void
cgi::printHeaders() const
{
	dodoStringMap::const_iterator i(HEADERS.begin()), j(HEADERS.end());
	for (; i != j; ++i)
		#ifdef FCGI_EXT
		if (cgiFastSet)
			cf->print(i->first + ": " + i->second + "\r\n");
		else
		#endif
		fstd->writeStreamString(i->first + ": " + i->second + "\r\n");

	if (cookiesSet.size() > 0)
	{
		std::list<__cookies>::const_iterator i(cookiesSet.begin()), j(cookiesSet.end());
		for (; i != j; ++i)
		{
			#ifdef FCGI_EXT
			if (cgiFastSet)
			{
				cf->print("Set-Cookie: ");
				cf->print(i->name + "=" + i->value + "; ");
				if (i->path.size() > 0)
					cf->print("path=" + i->path + "; ");
				if (i->exDate.size() > 0)
					cf->print("expires=" + i->exDate + "; ");
				if (i->domain.size() > 0)
					cf->print("domain=" + i->domain + "; ");
				if (i->secure)
					cf->print("secure");
			}
			else
			#endif
			{
				fstd->writeStreamString("Set-Cookie: ");
				fstd->writeStreamString(i->name + "=" + i->value + "; ");
				if (i->path.size() > 0)
					fstd->writeStreamString("path=" + i->path + "; ");
				if (i->exDate.size() > 0)
					fstd->writeStreamString("expires=" + i->exDate + "; ");
				if (i->domain.size() > 0)
					fstd->writeStreamString("domain=" + i->domain + "; ");
				if (i->secure)
					fstd->writeStreamString("secure");
			}
		}
	}

	#ifdef FCGI_EXT
	if (cgiFastSet)
	{
		cf->print("\r\n\r\n");
		cf->flush();
	}
	else
	#endif
	{
		fstd->writeStreamString("\r\n\r\n");
		fstd->flush();
	}
}

//-------------------------------------------------------------------

void
cgi::makeContent()
{
	unsigned long inSize = stringTools::stringToUL(ENVIRONMENT["CONTENT_LENGTH"]);

	if (inSize <= 0)
		return ;

	char *cont = new char[CONTENT_BATCH_SIZE];

	unsigned long iter = inSize / CONTENT_BATCH_SIZE;
	unsigned long rest = inSize % CONTENT_BATCH_SIZE;

	for (unsigned long i = 0; i < iter; ++i)
	{

		#ifdef FCGI_EXT
		if (cgiFastSet)
			cf->read(cont, CONTENT_BATCH_SIZE);
		else
		#endif
		if (fread(cont, CONTENT_BATCH_SIZE, 1, stdin) != CONTENT_BATCH_SIZE)
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					delete [] cont;

					return ;
			}

		content.append(cont, CONTENT_BATCH_SIZE);
	}

	if (rest > 0)
	{
		if (fread(cont, rest, 1, stdin) == 0)
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					delete [] cont;

					return ;
			}

		content.append(cont, rest);
	}

	delete [] cont;
}

//-------------------------------------------------------------------

void
cgi::makePost()
{
	if (content.size() == 0)
		return ;

	if (!stringTools::iequal(ENVIRONMENT["REQUEST_METHOD"], "POST"))
		return ;

	if (stringTools::iequal(ENVIRONMENT["CONTENT_TYPE"], "application/x-www-form-urlencoded"))
	{
		make(METHOD_POST, content);
	}
	else
	{
		if (stringTools::iequal(ENVIRONMENT["CONTENT_IOSOCKETOPTIONS_TRANSFER_ENCODING"], "base64"))
			content = tools::decodeBase64(content);

		unsigned int temp0;
		temp0 = ENVIRONMENT["CONTENT_TYPE"].find("boundary=");
		dodoStringArr postPartd = tools::explode(content, "--" + ENVIRONMENT["CONTENT_TYPE"].substr(temp0 + 9));

		dodoStringArr::iterator i(postPartd.begin()), j(postPartd.end());

		unsigned int temp1;
		char *ptr;
		int fd;
		unsigned short pathLength = postFilesTmpDir.size() + 18;

		for (; i != j; ++i)
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

				#ifndef __FreeBSD__

				if (!postFilesInMem)

				#endif
				{
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
				}

				file.size = i->substr(temp1 + 4).size() - 2;

				file.error = CGI_POSTFILEERR_NONE;

				#ifndef __FreeBSD__

				if (postFilesInMem)
				{
					file.buf = malloc(file.size);
					file.fp = fmemopen(file.buf, file.size, "w+");
				}
				else

				#endif
				{
					file.fp = fdopen(fd, "w+");
				}
				if (file.fp == NULL)
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
					}
				fwrite(i->substr(temp1 + 4).c_str(), file.size, 1, file.fp);
				if (errno == ENOMEM)
					file.error = CGI_POSTFILEERR_NO_SPACE;

				FILES.insert(post_name, file);
			}
			else
			{
				if ((temp0 = i->find("name=\"")) == dodoString::npos)
					continue;
				temp0 += 6;
				temp1 = i->find("\"", temp0);

				METHOD_POST.insert(i->substr(temp0, temp1 - temp0), i->substr(temp1 + 5, i->size() - temp1 - 7)); //FIXME: damned boundaries. I've chosen 5 by substitution; It have to be CR+LF, but no =(; 7 = 5+2 -> unknown 5 + (CR+LF)
			}
	}
}

//-------------------------------------------------------------------

const dodoStringMap &
cgi::operator[](short method)
{
	if (method == CGI_REQUESTMETHOD_POST)
		return METHOD_POST;
	else
		return METHOD_GET;
}

//-------------------------------------------------------------------

dodoString
cgi::request(const dodoString &varName,
			 short first)
{
	dodoString met0 = METHOD_GET[varName];
	dodoString met1 = METHOD_POST[varName];

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
	__cookies temp(secure);
	temp.name = name;
	temp.value = tools::encodeURL(value);
	temp.exDate = exDate;
	temp.path = path;
	temp.domain = domain;
	cookiesSet.push_back(temp);
}

//-------------------------------------------------------------------

void
cgi::setCookie(const __cookies &cookie)
{
	cookiesSet.push_back(cookie);
}

//-------------------------------------------------------------------

