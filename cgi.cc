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

std::string 
cgipp::__method::operator [](const std::string &varName)
{
	assocArr::iterator i(methodArr.begin()),j(methodArr.end());
	
	for (;i!=j;++i)
		if (strcmp(varName.c_str(),i->first.c_str()) == 0)
			return i->second;
	return "_undefined_";
}

//-------------------------------------------------------------------

cgipp::cgipp(bool silent, assocArr &a_headers)
{	
	initHeaders(a_headers);
	
	if (!silent)
		printHeaders();
	
	makeEnv();
	
	detectMethod();

	makePost();
	makeGet();
}

//-------------------------------------------------------------------

cgipp::~cgipp()
{
	
}

//-------------------------------------------------------------------

void 
cgipp::detectMethod()
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
cgipp::getMethod()
{
	return method;
}

//-------------------------------------------------------------------

void
cgipp::makeEnv()
{
	register int len = sizeof(HTTP_ENV)/sizeof(__statements);
	
	register char *env;
	
	for (register int i=0;i<len;++i)
	{
		env = getenv(HTTP_ENV[i].str);
		ENVIRONMENT.methodArr[HTTP_ENV[i].str] = (env==NULL)?"_undefined_":env;
	}
}

//-------------------------------------------------------------------

void
cgipp::makeGet()
{
	stringArr getPair = tools::explode(ENVIRONMENT.methodArr["QUERY_STRING"],"&");
	stringArr::iterator i(getPair.begin()), j(getPair.end());
	stringArr temp;
	for(;i!=j;++i)
	{
		temp = tools::explode(*i,&decode64,"=");
		if (temp.size() > 1)
			METHOD_GET.methodArr[temp[0]] = temp[1];
	}
}

//-------------------------------------------------------------------

void 
cgipp::initHeaders(assocArr &a_headers)
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
cgipp::printHeaders()
{
	assocArr::iterator i(HEADERS.begin()),j(HEADERS.end());
	
	for (;i!=j;++i)
		std::cout << i->first << ": " << i->second << "\n";
	std::cout << "\n\n";
	std::cout.flush();
}

//-------------------------------------------------------------------

void 
cgipp::makePost()
{
	if (strcasecmp(ENVIRONMENT["REQUEST_METHOD"].c_str(),"POST") != 0)
		return ;
	
	register int cl = atoi(ENVIRONMENT["CONTENT_LENGTH"].c_str());
	register char *post = new char[cl*size_of_char];
	fread(post,cl,1,stdin);
	
	if (strcasecmp(ENVIRONMENT["CONTENT_TYPE"].c_str(),"application/x-www-form-urlencoded")==0)
	{
		stringArr postPair = tools::explode(post,"&");
		delete [] post;	
		stringArr::iterator i(postPair.begin()), j(postPair.end());
		stringArr temp;
		for(;i!=j;++i)
		{
			temp = tools::explode(*i,&decode64,"=");
			if (temp.size() > 1)
				METHOD_POST.methodArr[temp[0]] = temp[1];
		}		
	}
	else
	{
		
		char *separator = new char[60*size_of_char];
		strncpy(separator,post,59);

		std::string bPost;
		bPost.assign(post,cl);
		delete [] post;
		
		stringArr postPartd = tools::explode(bPost,separator);
		
		stringArr::iterator i(postPartd.begin()),j(postPartd.end());
		
		stringArr pocket;
		regexp exp2;
		regexp exp1;
		
		exp2.match("Content-Disposition:[\\s]*form-data;[\\s]*name=\"(.*)\";[\\s]*filename=\"(.*)\"[\\s]*Content-Type:[\\s]*([a-zA-Z-/]*)[\\s]*(.*)","");		
		exp1.match("Content-Disposition:[\\s]*form-data;[\\s]*name=\"(.*)\"[\\s]*(.*)","");
		
		for (;i!=j;++i)
			if (exp2.reMatch(*i,pocket))///file matched
			{
				std::cout << pocket[3];
			}
			else
				if (exp1.reMatch(*i,pocket))
					METHOD_POST.methodArr[pocket[0]] = pocket[1];
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

char 
cgipp::hexToChar(const char &first,
				const char &second
				)
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
