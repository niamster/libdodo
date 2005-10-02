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
regexp::extended(bool a_extended) const
{
	extended_reg = a_extended;
}

//-------------------------------------------------------------------

void  
regexp::ignoreCase(bool a_ignore) const
{
	icase = a_ignore;
}

//-------------------------------------------------------------------

bool 
regexp::match(const std::string &pattern, 
				const std::string &sample, 
				stringArr &pockets) const
{
	if (!compile(pattern))
		return false;
	
	return reMatch(sample,pockets);
}

//-------------------------------------------------------------------

bool
regexp::reMatch(const std::string &sample, 
				stringArr &pockets) const
{
	pockets.clear();
	if (!boundMatch(sample))
		return false;
	std::vector<__regexMatch>::const_iterator i(boundaries.begin()),j(boundaries.end());
	for (;i!=j;++i)
		pockets.push_back(sample.substr(i->begin,i->end-i->begin));
}

//-------------------------------------------------------------------

bool
regexp::boundMatch(const std::string &sample) const
{
	register int subs, res;
	__regexMatch bound;
	boundaries.clear();
	
	#ifdef PCRE_EXT
		subs = pcre_info(code,NULL,NULL);
		if (subs<0)
			return false;
		subs *= 3;
		subs += 3;
		
		register int *oVector = new int[subs];
		register int rc = pcre_exec(code, NULL, sample.c_str(), sample.size(), 0, 0, oVector, subs);
		if (rc<=0)
		{
			delete [] oVector;
			return false;
		}
		register const char *subString;
		for (register int j=1;j<rc;++j)
		{
			res = pcre_get_substring(sample.c_str(),oVector,rc,j,&subString);
			if (res>0)
			{
				subs = j*2;
				bound.begin = oVector[subs];
				bound.end = oVector[subs+1];
				boundaries.push_back(bound);
			}
			else
			{
				bound.begin = 0;
				bound.end = 0;
				boundaries.push_back(bound);				
			}
			pcre_free_substring(subString);
		}
		delete [] oVector;
		return true;
	#else
		subs = code->re_nsub+1;
		regmatch_t *pmatch = new regmatch_t[subs];
		res = regexec(code,sample.c_str(),subs,pmatch,0);
		if (res != 0)
		{
			delete [] pmatch;
			return false;
		}
		for (register int i(1); i<subs;++i)
		{
			bound.begin = pmatch[i].rm_so;
			bound.end = pmatch[i].rm_eo;
			boundaries.push_back(bound);
		}
		delete [] pmatch;
		return true;
	#endif	
}

//-------------------------------------------------------------------

bool 
regexp::compile(const std::string &pattern) const
{
	register int bits(0);
	
	#ifdef PCRE_EXT
		if (extended_reg)
			bits|=PCRE_EXTENDED;
		if (icase)
			bits|=PCRE_CASELESS;
		bits|=PCRE_DOTALL;
		
		register int errOffset(0);
		register const char *error;
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
}

//-------------------------------------------------------------------

std::string 
regexp::replace(const std::string &pattern, 
				const std::string &sample, 
				const stringArr &replacements) const
{
	if (!compile(pattern))
		return false;
	
	return reReplace(sample,replacements);
}

//-------------------------------------------------------------------

std::string 
regexp::reReplace(const std::string &sample, 
				const stringArr &replacements) const
{
	if (!boundMatch(sample))
		return false;
	std::vector<__regexMatch>::const_iterator i(boundaries.begin()),j(boundaries.end());
	stringArr::const_iterator k(replacements.begin());
	register int amount = replacements.size();
	std::string temp(sample);
	for (register int o(0);o<amount && i!=j;++i,++o,++k)
		temp.replace(i->begin,i->end-i->begin,*k);
	return temp;
}

//-------------------------------------------------------------------
