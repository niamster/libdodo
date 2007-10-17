/***************************************************************************
 *            cgi.cc
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
 
#include "cgi.h"

using namespace dodo;

//-------------------------------------------------------------------

#define __UNDEFINED__ "_undefined_"

std::string
cgipp::__method::operator [](const std::string &varName)
{
	assocArr::iterator i(methodArr.begin()),j(methodArr.end());
	
	for (;i!=j;++i)
		if (strcmp(varName.c_str(),i->first.c_str()) == 0)
			return i->second;
	return __UNDEFINED__;
}

//-------------------------------------------------------------------

__cookies::__cookies(bool a_secure) : secure(a_secure)
{
}

cgipp::cgipp(bool silent, 
			assocArr &a_headers)
{	
	initHeaders(a_headers);
	
	if (!silent)
		printHeaders();
	
	makeEnv();
	
	detectMethod();

	makePost();
	make(COOKIES,ENVIRONMENT["HTTP_COOKIE"],"; ");
	make(METHOD_GET,ENVIRONMENT["QUERY_STRING"]);
}

//-------------------------------------------------------------------

cgipp::~cgipp()
{
	
}

//-------------------------------------------------------------------

void 
cgipp::cleanTmp() const
{
	std::map<std::string, __cgiFilesUp>::iterator i(postFiles.begin()),j(postFiles.end());
	for (;i!=j;++i)
		unlink(i->second.tmp_name.c_str());
}

//-------------------------------------------------------------------

void 
cgipp::detectMethod() const
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
cgipp::getMethod() const
{
	return method;
}

//-------------------------------------------------------------------

void 
cgipp::make(__method &val,
			const std::string &string,
			char *delim) const
{
	stringArr getPair = tools::explode(string,delim);
	stringArr::iterator i(getPair.begin()), j(getPair.end());
	stringArr temp;
	for(;i!=j;++i)
	{
		temp = tools::explode(*i,&decode64,"=");
		if (temp.size() > 1)
			val.methodArr[temp[0]] = temp[1];
	}	
}

//-------------------------------------------------------------------

void
cgipp::makeEnv() const
{
	register int len = sizeof(HTTP_ENV)/sizeof(__statements);
	
	register char *env;
	
	for (register int i=0;i<len;++i)
	{
		env = getenv(HTTP_ENV[i].str);
		ENVIRONMENT.methodArr[HTTP_ENV[i].str] = (env==NULL)?__UNDEFINED__:env;
	}
}

//-------------------------------------------------------------------

void 
cgipp::initHeaders(assocArr &a_headers) const
{
	HEADERS["Content-type"] = "text/html";
	HEADERS["X-Powered-By"] = LIBDODO_VERSION;
	
	if (a_headers.size() > 0)
	{
		assocArr::iterator i(a_headers.begin()),j(a_headers.end());
	
		for (;i!=j;++i)
			HEADERS[i->first] = i->second;	
	}
}

//-------------------------------------------------------------------

void 
cgipp::printHeaders() const
{
	assocArr::iterator i(HEADERS.begin()),j(HEADERS.end());
	
	for (;i!=j;++i)
		std::cout << i->first << ": " << i->second << "\n";
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
	std::cout << "\n\n";
	std::cout.flush();
}

//-------------------------------------------------------------------

void 
cgipp::makePost() const
{
	if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"POST") != 0)
		return ;
	
	register int cl = atoi(ENVIRONMENT["CONTENT_LENGTH"].c_str());
	register char *post = new char[(cl+1)*size_of_char];
	fread(post,cl,1,stdin);
	
	if (strcasecmp(ENVIRONMENT["CONTENT_TYPE"].c_str(),"application/x-www-form-urlencoded")==0)
	{
		make(METHOD_POST,post);
		delete [] post;	
	}
	else
	{
		std::string bPost;
		bPost.assign(post,cl);
		delete [] post;
		
		register int temp0;
		temp0 = ENVIRONMENT["CONTENT_TYPE"].find("boundary=");
		stringArr postPartd = tools::explode(bPost,"--"+ENVIRONMENT["CONTENT_TYPE"].substr(temp0+9));
		
		stringArr::iterator i(postPartd.begin()),j(postPartd.end());

		register int temp1;
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
				temp1 = i->find("\r\n",temp0);
				file.type = i->substr(temp0,temp1-temp0);
				
				std::string tmp_name = post_files_tmp_dir;
				tmp_name.append("dodo_post_XXXXXX");
				ptr = mktemp((char*)tmp_name.c_str());
				
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
								
				METHOD_POST.methodArr[i->substr(temp0,temp1-temp0)] = i->substr(temp1+5,i->size()-temp1-7);///damned boundaries. I've chosen 5 by substitution; It have to be CR+LF, but no =(; 7 = 5+2 -> unknown 5 + (CR+LF)
			}
	}	
}

//-------------------------------------------------------------------

std::string 
cgipp::decode64(const std::string &string)
{
	std::string result;
	std::string::const_iterator i(string.begin()),j(string.end());
	char c;

	for(;i!=j;++i) 
	{
		switch(*i) 
		{
			case '+':
				result.append(1, ' ');
				break;
			case '%':
				if(std::distance(i, j) >= 2 && std::isxdigit(*(i + 1)) && std::isxdigit(*(i + 2))) 
				{
					c = *++i;
					result.append(1, hexToChar(c, *++i));
				}
				else 
					result.append(1, '%');
				break;
			default:
				result.append(1, *i);
		}
	}
	return result;	
}

//-------------------------------------------------------------------

char 
cgipp::hexToChar(const char &first,
				const char &second)
{
	int val=0;
	switch (first)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			val = (16*(int(first)-48));
			break;
		default:
			val = (16*(int(first)-55));
	}  
	switch (second)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			val += (int(second)-48);
		break;
		default:
			val += (int(second)-55);
	}   
	return char(val);
}

//-------------------------------------------------------------------

__cgiFilesUp 
cgipp::getFile(const std::string &varName) const
{
	return postFiles[varName];
}

//-------------------------------------------------------------------

dodo::cgipp::__method &
cgipp::operator[](int method) const
{
	if (method == POST)
		return METHOD_POST;
	else
		return METHOD_GET;
}

//-------------------------------------------------------------------

std::string
cgipp::request(const std::string &varName, 
				int first) const
{
	std::string temp0 = METHOD_GET[varName];
	std::string temp1 = METHOD_POST[varName];
	
	if (first == GET)
		if (strcmp(temp0.c_str(),__UNDEFINED__) != 0)
			return temp0;
		else
			return temp1;
	else
		if (strcmp(temp1.c_str(),__UNDEFINED__) != 0)
			return temp1;
		else
			return temp0;
		
}

//-------------------------------------------------------------------

void 
cgipp::setCookie(const std::string &name, 
				const std::string &value, 
				const std::string &exDate, 
				const std::string &path, 
				const std::string &domain, 
				bool secure) const
{
	__cookies temp(secure);
	temp.name = name;
	temp.value = value;
	temp.exDate = exDate;
	temp.path = path;
	temp.domain = domain;
	cookiesSet.push_back(temp);
}

//-------------------------------------------------------------------
