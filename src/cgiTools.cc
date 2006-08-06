/***************************************************************************
 *            cgiTools.cc
 *
 *  Sat Sep  18 17:18:19 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <cgiTools.h>

#include <string.h>
#include <stdlib.h>

#include <cgiToolsEx.h>
#include <tools.h>


using namespace dodo;

bool cgiTools::cgiFilesInMem = true;

//-------------------------------------------------------------------

__cookies::__cookies(const std::string &a_name, 
					const std::string &a_value, 
					const std::string &a_exDate, 
					const std::string &a_path, 
					const std::string &a_domain, 
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
			assocArr &a_headers) : _cgiFilesInMem(true)
									#ifdef FCGI_EXT
										,
										cgiFastSet(false)
									#endif	
				
{		
	initHeaders(a_headers);
	
	if (!silent)
		printHeaders();
	
	makeEnv();
	
	detectMethod();

	makePost();
	make(COOKIES.realArr,ENVIRONMENT["HTTP_COOKIE"],"; ");
	make(METHOD_GET.realArr,ENVIRONMENT["QUERY_STRING"]);
	
	getPair.clear();
}

//-------------------------------------------------------------------

#ifdef FCGI_EXT

	cgiTools::cgiTools(cgiFastSTD *a_cf, 
						bool silent, 
						assocArr &a_headers) : _cgiFilesInMem(true),
											cgiFastSet(true),
											cf(a_cf)
					
	{		
		initHeaders(a_headers);
		
		if (!silent)
			printHeaders();
		
		makeEnv();
		
		detectMethod();
	
		makePost();
		make(COOKIES.realArr,ENVIRONMENT["HTTP_COOKIE"],"; ");
		make(METHOD_GET.realArr,ENVIRONMENT["QUERY_STRING"]);
		
		getPair.clear();
	}

#endif	

//-------------------------------------------------------------------

cgiTools::~cgiTools()
{
	cleanTmp();
}

//-------------------------------------------------------------------

void 
cgiTools::cleanTmp() const
{
	std::map<std::string, __cgiFilesUp>::iterator i(postFiles.begin()),j(postFiles.end());
	for (;i!=j;++i)
	{
		fclose(i->second.fp);
		
		if (_cgiFilesInMem)
			free(i->second.buf);
		else
			unlink(i->second.tmp_name.c_str());
	}
}

//-------------------------------------------------------------------

void 
cgiTools::detectMethod() const
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
cgiTools::make(assocArr &val,
			const std::string &string,
			char *delim) const
{	
	getPair = tools::explode(tools::decodeURL(string),delim);
	
	l = getPair.begin();
	m = getPair.end();
	
	stringArr temp;
	
	for(;l!=m;++l)
	{
		temp = tools::explode(*l,"=");
		if (temp.size() > 1)
			val[temp[0]] = temp[1];
	}	
}

//-------------------------------------------------------------------

void
cgiTools::makeEnv() const
{
	register char *env;
	
	for (register int i=0;i<HTTP_ENV_SIZE;++i)
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
cgiTools::initHeaders(assocArr &a_headers) const
{
	HEADERS["Content-type"] = "text/html";
	HEADERS["X-Powered-By"] = LIBRARY "/" MAJOR "." MINOR "." RELEASE ;
	
	if (a_headers.size() > 0)
	{
		i = a_headers.begin();
		j = a_headers.end();
	
		for (;i!=j;++i)
			HEADERS[i->first] = i->second;	
	}
}

//-------------------------------------------------------------------

void 
cgiTools::printHeaders() const
{
	 i = HEADERS.begin();
	 j = HEADERS.end();
	
	for (;i!=j;++i)
		#ifdef FCGI_EXT
			if (cgiFastSet)
				cf->print(i->first + ": " + i->second + "\r\n");
			else
		#endif		
				std::cout << i->first << ": " << i->second << "\r\n";
		
	if (cookiesSet.size()>0)
	{
		std::list<__cookies>::iterator i(cookiesSet.begin()),j(cookiesSet.end());
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
					std::cout << "Set-Cookie: ";
					std::cout << i->name << "=" << i->value << "; ";
					if (i->path.size() > 0)	
						std::cout << "path=" << i->path << "; ";
					if (i->exDate.size() > 0)	
						std::cout << "expires=" << i->exDate << "; ";
					if (i->domain.size() > 0)
						std::cout << "domain=" << i->domain << "; ";
					if (i->secure)
						std::cout << "secure";
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
			std::cout << "\r\n\r\n";
			std::cout.flush();
		}
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
cgiTools::makePost() const
{
	if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"POST") != 0)
		#ifndef NO_EX
			return ;
		#else
			return false;
		#endif
	
	register unsigned long inSize = atoi(ENVIRONMENT["CONTENT_LENGTH"].c_str());
	register char *post = new char[POST_BATCH_SIZE];	

	unsigned long iter = inSize/POST_BATCH_SIZE;
	unsigned long rest = inSize%POST_BATCH_SIZE;
	
	std::string bPost;
	
	_cgiFilesInMem = cgiFilesInMem;
			
	for (register unsigned long i=0;i<iter;++i)
	{
		
		#ifdef FCGI_EXT
			if (cgiFastSet)
				cf->read(post,POST_BATCH_SIZE);
			else
		#endif
				if (fread(post,POST_BATCH_SIZE,1,stdin) != POST_BATCH_SIZE)
					#ifndef NO_EX
						switch (errno)
						{
							case EIO:
							case EINTR:
							case ENOMEM:
							case EOVERFLOW:
							case EROFS:
								throw baseEx(ERRMODULE_CGITOOLS,CGITOOLS_MAKEPOST,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
		
		bPost.append(post,POST_BATCH_SIZE);
	}
	
	if (rest>0)
	{
		if (fread(post,rest,1,stdin) == 0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_CGITOOLS,CGITOOLS_MAKEPOST,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			
		bPost.append(post,rest);
	}

	delete [] post;
	
	if (strcasecmp(ENVIRONMENT["CONTENT_TYPE"].c_str(),"application/x-www-form-urlencoded")==0)
	{
		make(METHOD_POST.realArr,bPost);
	}
	else
	{
		if (strcasecmp(ENVIRONMENT["CONTENT_TRANSFER_ENCODING"].c_str(),"base64")==0)
			bPost = tools::decodeBase64(bPost);
		
		register unsigned int temp0;
		temp0 = ENVIRONMENT["CONTENT_TYPE"].find("boundary=");
		stringArr postPartd = tools::explode(bPost,"--"+ENVIRONMENT["CONTENT_TYPE"].substr(temp0+9));
		
		stringArr::iterator i(postPartd.begin()),j(postPartd.end());

		register unsigned int temp1;
		char *ptr = new char[postFilesTmpDir.size()+17];
		
		for (;i!=j;++i)
			if (i->find("filename")!=std::string::npos)///file
			{
				if ((temp0 = i->find("name=\"")) == std::string::npos)
					continue;
				temp0 += 6;	
				temp1 = i->find("\"",temp0);
				
				std::string post_name = i->substr(temp0,temp1-temp0);
				
				__cgiFilesUp file;
				
				temp0 = i->find("filename=\"",temp1);
				temp0 += 10;	
				temp1 = i->find("\"",temp0);
				if (temp0==temp1)
					continue;
				
				file.name = i->substr(temp0,temp1-temp0);
				
				temp0 = i->find("Content-Type: ",temp1);
				temp0 += 14;
				temp1 = i->find("\n",temp0);
				file.type = i->substr(temp0,temp1-temp0);
				
				if (cgiFilesInMem)
				{
					ptr = tempnam((postFilesTmpDir + FILE_DELIM).c_str(),"dodo_post_");
					
					if (ptr == NULL)	
					{
						file.error = POSTFILEERR_BAD_FILE_NAME;
						postFiles[post_name] = file;
						
						continue;
					}
							
					file.tmp_name = ptr;
				}		
							
				file.size = i->substr(temp1+4).size()-2;
				
				file.error = POSTFILEERR_NONE;
				
				if (cgiFilesInMem)
				{
					file.buf = malloc(file.size);
					file.fp = fmemopen(file.buf,file.size,"w+");
				}
				else
				{
					file.fp = fopen(ptr,"w+");				
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
					}
				fwrite(i->substr(temp1+4).c_str(),file.size,1,file.fp);
				if (errno == ENOMEM)
						file.error = POSTFILEERR_NO_SPACE;
				
				postFiles[post_name] = file;
			}
			else///simply post
			{
				if ((temp0 = i->find("name=\"")) == std::string::npos)
					continue;
				temp0 += 6;		
				temp1 = i->find("\"",temp0);
								
				METHOD_POST.realArr[i->substr(temp0,temp1-temp0)] = i->substr(temp1+5,i->size()-temp1-7);///damned boundaries. I've chosen 5 by substitution; It have to be CR+LF, but no =(; 7 = 5+2 -> unknown 5 + (CR+LF)
			}
	}
	
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

__cgiFilesUp 
cgiTools::getFile(const std::string &varName) const
{
	return postFiles[varName];
}

//-------------------------------------------------------------------

dodoStringMap &
cgiTools::operator[](short method) const
{
	if (method == REQUESTMETHOD_POST)
		return METHOD_POST;
	else
		return METHOD_GET;
}

//-------------------------------------------------------------------

std::string
cgiTools::request(const std::string &varName, 
				short first) const
{
	met0 = METHOD_GET[varName];
	met1 = METHOD_POST[varName];
	
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
cgiTools::setCookie(const std::string &name, 
				const std::string &value, 
				const std::string &exDate, 
				const std::string &path, 
				const std::string &domain, 
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

