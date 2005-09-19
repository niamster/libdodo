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
//	makePost();
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
		temp = tools::explode(*i,"=");
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
}

//-------------------------------------------------------------------

