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

using namespace dodo;

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
			assocArr &a_headers)
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
		unlink(i->second.tmp_name.c_str());
}

//-------------------------------------------------------------------

void 
cgiTools::detectMethod() const
{
	if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"GET") == 0)
		method = GET;
	else 
	{
		if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"POST") == 0 && ENVIRONMENT["REQUEST_METHOD"].empty())
			method = POST;
		else
			method = GET_POST;
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
	getPair = tools::explode(string,delim);
	l = getPair.begin();
	m = getPair.end();
	
	stringArr temp;
	
	for(;l!=m;++l)
	{
		temp = tools::explode(*l,&tools::decodeURL,"=");
		if (temp.size() > 1)
			val[temp[0]] = temp[1];
	}	
}

//-------------------------------------------------------------------

void
cgiTools::makeEnv() const
{
	register int len = sizeof(HTTP_ENV)/sizeof(__statements);
	
	register char *env;
	
	for (register int i=0;i<len;++i)
	{
		env = getenv(HTTP_ENV[i].str);
		ENVIRONMENT.realArr[HTTP_ENV[i].str] = (env==NULL)?__UNDEFINED__:env;
	}
}

//-------------------------------------------------------------------

void 
cgiTools::initHeaders(assocArr &a_headers) const
{
	HEADERS["Content-type"] = "text/html";
	HEADERS["X-Powered-By"] = LIBDODO_VERSION;
	
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
		std::cout << i->first << ": " << i->second << std::endl;
		
	if (cookiesSet.size()>0)
	{
		std::vector<__cookies>::iterator i(cookiesSet.begin()),j(cookiesSet.end());
		for (;i!=j;++i)
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
	std::cout << std::endl << std::endl;
	std::cout.flush();
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
	
	register int cl = atoi(ENVIRONMENT["CONTENT_LENGTH"].c_str());
	register char *post = new char[(cl+1)*size_of_char];	
	
	fread(post,cl,1,stdin);
	
	if (strcasecmp(ENVIRONMENT["CONTENT_TYPE"].c_str(),"application/x-www-form-urlencoded")==0)
	{
		make(METHOD_POST.realArr,post);
		delete [] post;	
	}
	else
	{
		std::string bPost;
		bPost.assign(post,cl);
		delete [] post;
		
		register unsigned int temp0;
		temp0 = ENVIRONMENT["CONTENT_TYPE"].find("boundary=");
		stringArr postPartd = tools::explode(bPost,"--"+ENVIRONMENT["CONTENT_TYPE"].substr(temp0+9));
		
		stringArr::iterator i(postPartd.begin()),j(postPartd.end());

		register unsigned int temp1;
		char *ptr = new char[strlen(post_files_tmp_dir)+17];
		
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
				
				ptr = tempnam((post_files_tmp_dir + std::string(1,FILE_DELIM)).c_str(),"dodo_post_");
				
				if (ptr == NULL)	
					continue;
							
				file.tmp_name = ptr;
							
				file.size = i->substr(temp1+4).size()-2;
				
				file.error = NONE;
				
				FILE *tmp = fopen(ptr,"w+b");
				if (tmp == NULL)
					switch(errno)
					{
						case EACCES:
						case EISDIR:
							file.error = ACCESS_DENY;
							break;
						case ENAMETOOLONG:
						case ENOTDIR:
							file.error = BAD_FILE_NAME;
							break;
						case ENOMEM:
							file.error = NO_SPACE;
					}
				free(ptr);
				fwrite(i->substr(temp1+4).c_str(),file.size,1,tmp);
				if (errno == ENOMEM)
						file.error = NO_SPACE;
				fclose(tmp);
				
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
}

//-------------------------------------------------------------------

__cgiFilesUp 
cgiTools::getFile(const std::string &varName) const
{
	return postFiles[varName];
}

//-------------------------------------------------------------------

dodoMap &
cgiTools::operator[](requestMethodEnum method) const
{
	if (method == POST)
		return METHOD_POST;
	else
		return METHOD_GET;
}

//-------------------------------------------------------------------

std::string
cgiTools::request(const std::string &varName, 
				requestMethodEnum first) const
{
	met0 = METHOD_GET[varName];
	met1 = METHOD_POST[varName];
	
	if (first == GET)
		if (strcmp(met0.c_str(),__UNDEFINED__) != 0)
			return met0;
		else
			return met1;
	else
		if (strcmp(met1.c_str(),__UNDEFINED__) != 0)
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

