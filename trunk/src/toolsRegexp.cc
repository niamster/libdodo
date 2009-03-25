/***************************************************************************
 *            toolsRegexp.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#ifdef PCRE_EXT
#include <pcre.h>
#else
#include <sys/types.h>
#include <regex.h>
#endif

#include <libdodo/toolsRegexp.h>
#include <libdodo/types.h>
#include <libdodo/toolsRegexpEx.h>

using namespace dodo::tools;

regexp::regexp(regexp &rt)
{
}

//-------------------------------------------------------------------

regexp::regexp() : extended(true),
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

regexp::regexp(const dodoString &pattern) : extended(true),
											icase(false),
											greedy(true),
											multiline(false)
{
#ifdef PCRE_EXT
#else
	notCompiled = true;
#endif

	compile(pattern);
}

//-------------------------------------------------------------------

regexp::~regexp()
{
#ifdef PCRE_EXT
#else
	if (!notCompiled)
	{
		regfree(&code);
	}
#endif
}

//-------------------------------------------------------------------

bool
regexp::match(const dodoString &pattern,
			  const dodoString &sample,
			  dodoStringArray  &pockets)
{
	try
	{
		compile(pattern);
	}
	catch (exception::basic &ex)
	{
		if (ex.funcID == REGEXPEX_COMPILE)
		{
			return false;
		}
		else
		{
			throw;
		}
	}

	return match(sample, pockets);
}

//-------------------------------------------------------------------

bool
regexp::match(const dodoString &sample,
			  dodoStringArray  &pockets)
{
	pockets.clear();
	if (!boundMatch(sample))
	{
		return false;
	}

#ifndef USE_DEQUE
	pockets.reserve(boundaries.size());
#endif

	dodoArray<__regexMatch__>::const_iterator i(boundaries.begin()), j(boundaries.end());
	for (; i != j; ++i)
	{
		pockets.push_back(dodoString(sample.data() + i->begin, i->end - i->begin));
	}

	return true;
}

//-------------------------------------------------------------------

bool
regexp::boundMatch(const dodoString &sample)
{
	boundaries.clear();

#ifdef PCRE_EXT
	int subs;

	if (pcre_fullinfo(code, NULL, PCRE_INFO_CAPTURECOUNT, &subs) != 0)
	{
		return false;
	}

	subs *= 3;
	subs += 3;

	int *oVector = new int[subs];
	int rc = pcre_exec(code, NULL, sample.c_str(), sample.size(), 0, 0, oVector, subs);
	if (rc <= 0)
	{
		delete [] oVector;

		return false;
	}

	__regexMatch__ bound;

	for (int j = 1; j < rc; ++j)
	{
		subs = j * 2;
		bound.begin = oVector[subs];
		bound.end = oVector[subs + 1];
		boundaries.push_back(bound);
	}

	delete [] oVector;

	return true;
#else
	int subs = code.re_nsub + 1;
	regmatch_t *pmatch = new regmatch_t[subs];

	int res = regexec(&code, sample.c_str(), subs, pmatch, 0);
	if (res != 0)
	{
		delete [] pmatch;
		return false;
	}

	__regexMatch__ bound;

	for (int i(1); i < subs; ++i)
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

void
regexp::compile(const dodoString &pattern)
{
	int bits(0);

#ifdef PCRE_EXT
	if (icase)
	{
		bits |= PCRE_CASELESS;
	}
	if (!greedy)
	{
		bits |= PCRE_UNGREEDY;
	}
	if (multiline)
	{
		bits |= PCRE_MULTILINE;
	}
	bits |= PCRE_DOTALL;

	int errOffset(0), errn(0);
	const char *error;
	code = pcre_compile2(pattern.c_str(), bits, &errn, &error, &errOffset, NULL);
	if (code == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSREGEXP, REGEXPEX_COMPILE, exception::ERRNO_PCRE, errn, error, __LINE__, __FILE__, pattern);
	}
#else
	if (extended)
	{
		bits |= REG_EXTENDED;
	}
	if (icase)
	{
		bits |= REG_ICASE;
	}

	if (notCompiled)
	{
		notCompiled = false;
	}
	else
	{
		regfree(&code);
	}

	int errn = regcomp(&code, pattern.c_str(), bits);
	if (errn != 0)
	{
#define ERROR_LEN 256
		char error[ERROR_LEN];
		regerror(errn, &code, error, ERROR_LEN);
		throw exception::basic(exception::ERRMODULE_TOOLSREGEXP, REGEXPEX_COMPILE, exception::ERRNO_POSIXREGEX, errn, error, __LINE__, __FILE__, pattern);
	}
#endif
}

//-------------------------------------------------------------------

dodoString
regexp::replace(const dodoString      &pattern,
				const dodoString      &sample,
				const dodoStringArray &replacements)
{
	try
	{
		compile(pattern);
	}
	catch (exception::basic &ex)
	{
		if (ex.funcID == REGEXPEX_COMPILE)
		{
			return sample;
		}
		else
		{
			throw;
		}
	}

	return replace(sample, replacements);
}

//-------------------------------------------------------------------

dodoString
regexp::replace(const dodoString      &sample,
				const dodoStringArray &replacements)
{
	if (!boundMatch(sample))
	{
		return sample;
	}

	dodoArray<__regexMatch__>::const_iterator i(boundaries.begin()), j(boundaries.end()), o;

	dodoStringArray::const_iterator k(replacements.begin());
	int subs = replacements.size();

	dodoString temp = sample;

	long shift = 0;
	unsigned long begin(0), end(0);

	for (int res = 0; res < subs && i != j; ++i, ++res, ++k)
	{
		if (res > 0)
		{
			o = i - 1;
			shift = o->end - o->begin - (k - 1)->size();
		}
		begin = i->begin - shift;
		end = i->end - shift;

		temp.replace(begin, end - begin, *k);
	}

	return temp;
}

//-------------------------------------------------------------------

