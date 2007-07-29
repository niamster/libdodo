/***************************************************************************
 *            cgiTools.cc
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
 
#include <cgiTools.h>

using namespace dodo;

bool cgiTools::postFilesInMem = true;

//-------------------------------------------------------------------

bool cgiTools::autoclearContent = true;

//-------------------------------------------------------------------

dodoString cgiTools::postFilesTmpDir = "/tmp";

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

cgiTools::cgiTools(cgiTools &ct)
{
}

//-------------------------------------------------------------------

cgiTools::cgiTools(bool silent, 
			dodoAssocArr &a_headers) : postFilesStoredInMem(true)
									#ifdef FCGI_EXT
										,
										cgiFastSet(false)
									#endif	
				
{		
	fstd = new flushSTD;

	initHeaders(a_headers);
	
	if (!silent)
		printHeaders();
	
	makeEnv();
	
	detectMethod();

	makePost();
	
	if(autoclearContent)
		content.clear();
	
	make(COOKIES.realArr, ENVIRONMENT["HTTP_COOKIE"],"; ");
	make(METHOD_GET.realArr, ENVIRONMENT["QUERY_STRING"]);
}

//-------------------------------------------------------------------

#ifdef FCGI_EXT

	cgiTools::cgiTools(cgiFastSTD *a_cf, 
						bool silent, 
						dodoAssocArr &a_headers) : postFilesStoredInMem(true),
											cgiFastSet(true),
											cf(a_cf)
					
	{		
		initHeaders(a_headers);
		
		if (!silent)
			printHeaders();
		
		makeEnv();
		
		detectMethod();
	
		makePost();
		make(COOKIES.realArr, ENVIRONMENT["HTTP_COOKIE"],"; ");
		make(METHOD_GET.realArr, ENVIRONMENT["QUERY_STRING"]);
	}

#endif	

//-------------------------------------------------------------------

cgiTools::~cgiTools()
{
	cleanTmp();
	
	if (!cgiFastSet)
		delete fstd;
}

//-------------------------------------------------------------------

dodoString 
cgiTools::getContent()
{
	return content;
}

//-------------------------------------------------------------------

void
cgiTools::clearContent()
{
	content.clear();
}

//-------------------------------------------------------------------

void 
cgiTools::cleanTmp()
{
	std::map<dodoString, __cgiFilesUp>::iterator i(FILES.begin()), j(FILES.end());
	for (;i!=j;++i)
	{
		fclose(i->second.fp);
		
		if (postFilesStoredInMem)
			free(i->second.buf);
		else
			unlink(i->second.tmp_name.c_str());
	}
}

//-------------------------------------------------------------------

void 
cgiTools::detectMethod()
{
	if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"GET") == 0)
		method = REQUESTMETHOD_GET;
	else 
	{
		if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"POST") == 0 && ENVIRONMENT["REQUEST_METHOD"].empty())
			method = REQUESTMETHOD_POST;
		else
			method = REQUESTMETHOD_GET_POST;
	}
}

//-------------------------------------------------------------------

int 
cgiTools::getMethod() const
{
	return method;
}

//-------------------------------------------------------------------

void 
cgiTools::make(dodoAssocArr &val,
			const dodoString &string,
			char *delim)
{	
	dodoStringArr getPair = tools::explode(tools::decodeURL(string), delim);
	
	dodoStringArr::iterator l(getPair.begin()), m(getPair.end());
	
	dodoStringArr temp;
	
	for(;l!=m;++l)
	{
		temp = tools::explode(*l,"=");
		if (temp.size() > 1)
			val[temp[0]] = temp[1];
	}	
}

//-------------------------------------------------------------------

void
cgiTools::makeEnv()
{
	char *env;
	
	for (int i=0;i<HTTP_ENV_SIZE;++i)
	{
		#ifdef FCGI_EXT
			if (cgiFastSet)
				env = cf->getenv(HTTP_ENV[i].str);
			else
		#endif	
				env = getenv(HTTP_ENV[i].str);
				
		ENVIRONMENT.realArr[HTTP_ENV[i].str] = (env==NULL)?"NULL":env;
	}
}

//-------------------------------------------------------------------

void 
cgiTools::initHeaders(dodoAssocArr &a_headers)
{
	HEADERS["Content-type"] = "text/html";
	HEADERS["X-Powered-By"] = PACKAGE_NAME "/" PACKAGE_VERSION ;
	
	if (a_headers.size() > 0)
	{
		dodoAssocArr::iterator i(a_headers.begin()), j(a_headers.end());
		for (;i!=j;++i)
			HEADERS[i->first] = i->second;	
	}
}

//-------------------------------------------------------------------

void 
cgiTools::printHeaders() const
{
	dodoAssocArr::const_iterator i(HEADERS.begin()), j(HEADERS.end());
	for (;i!=j;++i)
		#ifdef FCGI_EXT
			if (cgiFastSet)
				cf->print(i->first + ": " + i->second + "\r\n");
			else
		#endif		
				fstd->writeStreamString(i->first + ": " + i->second + "\r\n");
		
	if (cookiesSet.size() > 0)
	{
		std::list<__cookies>::const_iterator i(cookiesSet.begin()), j(cookiesSet.end());
		for (;i!=j;++i)
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

#ifndef NO_EX
	void 
#else
	bool
#endif
cgiTools::makePost()
{
	if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"POST") != 0)
		#ifndef NO_EX
			return ;
		#else
			return false;
		#endif
	
	unsigned long inSize = atoi(ENVIRONMENT["CONTENT_LENGTH"].c_str());
	char *post = new char[POST_BATCH_SIZE];	

	unsigned long iter = inSize/POST_BATCH_SIZE;
	unsigned long rest = inSize%POST_BATCH_SIZE;
	
	postFilesStoredInMem = postFilesInMem;
			
	for (unsigned long i=0;i<iter;++i)
	{
		
		#ifdef FCGI_EXT
			if (cgiFastSet)
				cf->read(post, POST_BATCH_SIZE);
			else
		#endif
				if (fread(post, POST_BATCH_SIZE, 1, stdin) != POST_BATCH_SIZE)
					#ifndef NO_EX
						switch (errno)
						{
							case EIO:
							case EINTR:
							case ENOMEM:
							case EOVERFLOW:
							case EROFS:
							
								throw baseEx(ERRMODULE_CGITOOLS, CGITOOLS_MAKEPOST, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
						}	
					#else			
						switch (errno)
						{
							case EIO:
							case EINTR:
							case ENOMEM:
							case EOVERFLOW:	
							case EROFS:
							
								return false;
						}
					#endif
		
		content.append(post, POST_BATCH_SIZE);
	}
	
	if (rest>0)
	{
		if (fread(post, rest, 1, stdin) == 0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
					
						throw baseEx(ERRMODULE_CGITOOLS, CGITOOLS_MAKEPOST, ERR_ERRNO, errno, strerror(errno),__LINE__,__FILE__);
				}	
			#else			
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:	
					case EROFS:
					
						return false;
				}
			#endif
			
		content.append(post, rest);
	}

	delete [] post;
	
	if (strcasecmp(ENVIRONMENT["CONTENT_TYPE"].c_str(),"application/x-www-form-urlencoded")==0)
	{
		make(METHOD_POST.realArr, content);
	}
	else
	{
		if (strcasecmp(ENVIRONMENT["CONTENT_TRANSFER_ENCODING"].c_str(),"base64")==0)
			content = tools::decodeBase64(content);
		
		unsigned int temp0;
		temp0 = ENVIRONMENT["CONTENT_TYPE"].find("boundary=");
		dodoStringArr postPartd = tools::explode(content,"--"+ENVIRONMENT["CONTENT_TYPE"].substr(temp0+9));
		
		dodoStringArr::iterator i(postPartd.begin()), j(postPartd.end());

		unsigned int temp1;
		char *ptr;
		int fd;
		unsigned short pathLength = postFilesTmpDir.size()+18;
		
		for (;i!=j;++i)
			if (i->find("filename")!=dodoString::npos)
			{
				if ((temp0 = i->find("name=\"")) == dodoString::npos)
					continue;
				temp0 += 6;	
				temp1 = i->find("\"", temp0);
				
				dodoString post_name = i->substr(temp0, temp1-temp0);
				
				__cgiFilesUp file;
				
				temp0 = i->find("filename=\"", temp1);
				temp0 += 10;	
				temp1 = i->find("\"", temp0);
				if (temp0==temp1)
					continue;
				
				file.name = i->substr(temp0, temp1-temp0);
				
				temp0 = i->find("Content-Type: ", temp1);
				temp0 += 14;
				temp1 = i->find("\n", temp0);
				file.type = i->substr(temp0, temp1-temp0);
				
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
						
						file.error = POSTFILEERR_BAD_FILE_NAME;
						FILES.realArr[post_name] = file;
						
						continue;
					}
							
					file.tmp_name = ptr;
					
					delete [] ptr;
				}		
							
				file.size = i->substr(temp1+4).size()-2;
				
				file.error = POSTFILEERR_NONE;
				
				#ifndef __FreeBSD__

					if (postFilesInMem)
					{
						file.buf = malloc(file.size);
						file.fp = fmemopen(file.buf, file.size,"w+");
					}
					else

				#endif
				{
					file.fp = fdopen(fd,"w+");				
					free(ptr);
				}
				if (file.fp == NULL)
					switch(errno)
					{
						case EACCES:
						case EISDIR:
						
							file.error = POSTFILEERR_ACCESS_DENY;
							
							break;
							
						case ENAMETOOLONG:
						case ENOTDIR:
						
							file.error = POSTFILEERR_BAD_FILE_NAME;
							break;
							
						case ENOMEM:
						
							file.error = POSTFILEERR_NO_SPACE;
							
							break;
					}
				fwrite(i->substr(temp1+4).c_str(), file.size, 1, file.fp);
				if (errno == ENOMEM)
						file.error = POSTFILEERR_NO_SPACE;
				
				FILES.realArr[post_name] = file;
			}
			else
			{
				if ((temp0 = i->find("name=\"")) == dodoString::npos)
					continue;
				temp0 += 6;		
				temp1 = i->find("\"", temp0);
								
				METHOD_POST.realArr[i->substr(temp0, temp1-temp0)] = i->substr(temp1+5, i->size()-temp1-7);//FIXME: damned boundaries. I've chosen 5 by substitution; It have to be CR+LF, but no =(; 7 = 5+2 -> unknown 5 + (CR+LF)
			}
	}
	
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

dodoStringMap &
cgiTools::operator[](short method)
{
	if (method == REQUESTMETHOD_POST)
		return METHOD_POST;
	else
		return METHOD_GET;
}

//-------------------------------------------------------------------

dodoString
cgiTools::request(const dodoString &varName, 
				short first)
{
	dodoString met0 = METHOD_GET[varName];
	dodoString met1 = METHOD_POST[varName];
	
	if (first == REQUESTMETHOD_GET)
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
cgiTools::setCookie(const dodoString &name, 
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
cgiTools::setCookie(const __cookies &cookie)
{
	cookiesSet.push_back(cookie);	
}

//-------------------------------------------------------------------

