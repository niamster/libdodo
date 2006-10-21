/***************************************************************************
 *            regexpTools.cc
 *
 *  Thu Sep 20 01:43:24 2005
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
 
#include <regexpTools.h>

using namespace dodo;

regexpTools::regexpTools(regexpTools &rt)
{
}

//-------------------------------------------------------------------

regexpTools::regexpTools() : extended(true),
					icase(false),
					greedy(true),
					multiline(false)
{
	#ifdef PCRE_EXT
	#else
		notCompiled = true;
	#endif		
}

//-------------------------------------------------------------------

regexpTools::~regexpTools()
{
	#ifdef PCRE_EXT
	#else
		if (!notCompiled)
			regfree(&code);
	#endif	
}

//-------------------------------------------------------------------

bool 
regexpTools::match(const std::string &pattern, 
				const std::string &sample, 
				dodoStringArr &pockets) const
{
	if (!compile(pattern))
		return false;

	return reMatch(sample,pockets);
}

//-------------------------------------------------------------------

bool
regexpTools::reMatch(const std::string &sample, 
				dodoStringArr &pockets) const
{
	pockets.clear();
	if (!boundMatch(sample))
		return false;
	
	#ifndef USE_DEQUE
		pockets.reserve(boundaries.size());
	#endif
		
	i = boundaries.begin();
	j = boundaries.end();
	for (;i!=j;++i)
		pockets.push_back(sample.substr(i->begin,i->end-i->begin));
		
	return true;
}

//-------------------------------------------------------------------

bool
regexpTools::boundMatch(const std::string &sample) const
{
	boundaries.clear();
	
	#ifdef PCRE_EXT
	
		if (pcre_fullinfo(code,NULL,PCRE_INFO_CAPTURECOUNT,&subs) != 0)
			return false;
			
		subs *= 3;
		subs += 3;
		
		register int *oVector = new int[subs];
			
		register int rc = pcre_exec(code, NULL, sample.c_str(), sample.size(), 0, 0, oVector, subs);
		if (rc <= 0)
		{
			delete [] oVector;
			return false;
		}
		
		for (register int j=1;j<rc;++j)
		{
			subs = j*2;
			bound.begin = oVector[subs];
			bound.end = oVector[subs+1];
			boundaries.push_back(bound);
		}
		
		delete [] oVector;
		
		return true;
		
	#else
	
		subs = code.re_nsub+1;
		regmatch_t *pmatch = new regmatch_t[subs];
				
		res = regexec(&code,sample.c_str(),subs,pmatch,0);
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
regexpTools::compile(const std::string &pattern) const
{
	register int bits(0);
	
	#ifdef PCRE_EXT
	
		if (extended)
			bits |= PCRE_EXTENDED;
		if (icase)
			bits |= PCRE_CASELESS;
		if (!greedy)	
			bits |= PCRE_UNGREEDY;
		if (multiline)
			bits |= PCRE_MULTILINE;
		bits |= PCRE_DOTALL;	
		
		register int errOffset(0);
		register const char *error;
		code = pcre_compile(pattern.c_str(), bits, &error, &errOffset, NULL);
		if (code == NULL)
			return false;
			
	#else
	
		if (extended)
			bits |= REG_EXTENDED;
		if (icase)
			bits |= REG_ICASE;
			
		if (notCompiled)
			notCompiled = false;
		else
			regfree(&code);
		if (regcomp(&code, pattern.c_str(),bits) != 0)
			return false;
			
	#endif
	
	return true;
}

//-------------------------------------------------------------------

std::string 
regexpTools::replace(const std::string &pattern, 
				const std::string &sample, 
				const dodoStringArr &replacements) const
{
	if (!compile(pattern))
		return sample;
	
	return reReplace(sample,replacements);
}

//-------------------------------------------------------------------

std::string 
regexpTools::reReplace(const std::string &sample, 
				const dodoStringArr &replacements) const
{
	if (!boundMatch(sample))
		return sample;
	
	i = boundaries.begin();
	j = boundaries.end();
	
	dodoStringArr::const_iterator k(replacements.begin());
	subs = replacements.size();
	
	temp.assign(sample);
	
	for (res = 0;res<subs && i!=j;++i,++res,++k)
		temp.replace(i->begin,i->end-i->begin,*k);
		
	return temp;
}

//-------------------------------------------------------------------
