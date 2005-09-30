/***************************************************************************
 *            regexp.cc
 *
 *  Thu Sep 20 01:43:24 2005
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
 
#include "regexp.h"

using namespace dodo;

regexp::regexp() : icase(false),
					extended_reg(true)
{
	#ifdef PCRE_EXT
		
	#else
		code = new regex_t;
	#endif		
}

//-------------------------------------------------------------------

regexp::~regexp()
{
	#ifdef PCRE_EXT
		
	#else
		regfree(code);
		delete code;
	#endif	
}

//-------------------------------------------------------------------

void 
regexp::extended(bool a_extended)
{
	extended_reg = a_extended;
}

//-------------------------------------------------------------------

void  
regexp::ignoreCase(bool a_ignore)
{
	icase = a_ignore;
}

//-------------------------------------------------------------------

bool 
regexp::match(const std::string &pattern, 
				const std::string &sample, 
				stringArr &pockets)
{
	int bits(0);
	
	#ifdef PCRE_EXT
		if (extended_reg)
			bits|=PCRE_EXTENDED;
		if (icase)
			bits|=PCRE_CASELESS;
		bits|=PCRE_DOTALL;
		
		int errOffset(0);
		const char *error;
		code = pcre_compile(pattern.c_str(), bits, &error, &errOffset, NULL);
		if (code == NULL)
			return false;
	#else
		if (extended_reg)
			bits|=REG_EXTENDED;
		if (icase)
			bits|=REG_ICASE;
		if (regcomp(code, pattern.c_str(),bits) != 0)
			return false;
	#endif
	
	return reMatch(sample,pockets);
}

//-------------------------------------------------------------------

bool
regexp::reMatch(const std::string &sample, 
				stringArr &pockets)
{
	pockets.clear();
	int subs;
	
	#ifdef PCRE_EXT
		subs = pcre_info(code,NULL,NULL);
		if (subs<0)
			return false;
		subs *= 3;
		subs += 3;
		
		int *oVector = new int[subs];
		int rc = pcre_exec(code, NULL, sample.c_str(), sample.size(), 0, 0, oVector, subs);
		if (rc<=0)
			return false;
		const char *subString;
		int res;
		std::string temp;
		for (int j=1;j<rc;++j)
		{
			res = pcre_get_substring(sample.c_str(),oVector,rc,j,&subString);
			if (res>0)
			{
				subs = j*2;
				temp.assign(subString,oVector[subs+1]-oVector[subs]);
				pockets.push_back(temp);
			}
			else
				return false;
		}
	#else
		subs = code->re_nsub+1;
		regmatch_t *pmatch = new regmatch_t[subs];
		regexec(code,sample.c_str(),subs,pmatch,0);
		for (int i(1); i<subs;++i)
			std::cout << "!" << pmatch[i].rm_so << "!" << pmatch[i].rm_eo << "\n";
		delete [] pmatch;
	#endif
}

//-------------------------------------------------------------------

/*std::string 
regexp::replace(const std::string &pattern, 
				const std::string &sample, 
				const stringArr &matches)
{
	#ifdef PCRE_EXT
	
	#else
	
	#endif	
}*/

//-------------------------------------------------------------------
