/***************************************************************************
 *            toolsMisc.cc
 *
 *  Wed Aug 24 00:43:21 2005
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

#include <libdodo/toolsMisc.h>

using namespace dodo::tools;

dodoString
dummyEscape(const dodoString &data)
{
	return data;
}

//-------------------------------------------------------------------

void
misc::random(void *data,
			 unsigned long size,
			 short strength)
{
	FILE *file;

	if (strength == MISC_RANDOMSTRENGTH_DEFAULT)
	{
		file = fopen("/dev/urandom", "r");
		if (file == NULL)
			throw exception::basic(exception::ERRMODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		if (strength == MISC_RANDOMSTRENGTH_STRONG)
		{
			file = fopen("/dev/random", "r");
			if (file == NULL)
				throw exception::basic(exception::ERRMODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		else
			throw exception::basic(exception::ERRMODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_LIBDODO, MISCEX_WRONGSTRENGTH, TOOLSMISCEX_WRONGSTRENGTH_STR, __LINE__, __FILE__);
	}

	while (true)
	{
		if (fread(data, size, 1, file) == 0)
		{
			if (feof(file) != 0 || errno == EAGAIN)
				break;

			if (errno == EINTR)
				continue;

			if (ferror(file) != 0)
				throw exception::basic(exception::ERRMODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}

		break;
	}

	if (fclose(file) != 0)
		throw exception::basic(exception::ERRMODULE_TOOLSMISC, MISCEX_RANDOM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

dodoString
misc::stringRandom(unsigned long size,
				   short strength)
{
	char *data = new char[size + 1];
	try
	{
		random(data, size, strength);
	}
	catch (...)
	{
		delete [] data;

		throw;
	}
	for (unsigned long i = 0; i < size; ++i)
		if (data[i] == '\0')
			data[i] = '*';

	dodoString res(data, size);
	delete [] data;

	return res;
}

//-------------------------------------------------------------------

unsigned long
misc::ulRandom(short strength)
{
	unsigned long rnd;

	random(&rnd, sizeof(unsigned long), strength);

	return rnd;
}

//-------------------------------------------------------------------

long
misc::lRandom(short strength)
{
	long rnd;

	random(&rnd, sizeof(long), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned int
misc::uiRandom(short strength)
{
	unsigned int rnd;

	random(&rnd, sizeof(unsigned int), strength);

	return rnd;
}

//-------------------------------------------------------------------

int
misc::iRandom(short strength)
{
	int rnd;

	random(&rnd, sizeof(int), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned short
misc::usRandom(short strength)
{
	unsigned short rnd;

	random(&rnd, sizeof(unsigned short), strength);

	return rnd;
}

//-------------------------------------------------------------------

short
misc::sRandom(short strength)
{
	short rnd;

	random(&rnd, sizeof(short), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned char
misc::ucRandom(short strength)
{
	unsigned char rnd;

	random(&rnd, sizeof(unsigned char), strength);

	return rnd;
}

//-------------------------------------------------------------------

char
misc::cRandom(short strength)
{
	char rnd;

	random(&rnd, sizeof(char), strength);

	return rnd;
}

//-------------------------------------------------------------------

double
misc::dRandom(short strength)
{
	double rnd;

	random(&rnd, sizeof(double), strength);

	return rnd;
}

//-------------------------------------------------------------------

bool
misc::isInArray(const dodoStringArray &arr,
				const dodoString &needle,
				bool icase)
{
	bool (*cmpFunc)(const dodoString &, const dodoString &);

	if (icase)
		cmpFunc = string::iequal;
	else
		cmpFunc = string::equal;

	dodoStringArray::const_iterator i(arr.begin()), j(arr.end());
	for (; i != j; ++i)
		if (cmpFunc(*i, needle))
			return true;

	return false;
}

//-------------------------------------------------------------------

bool
misc::isInList(const dodoStringList &arr,
			   const dodoString &needle,
			   bool icase)
{
	bool (*cmpFunc)(const dodoString &, const dodoString &);

	if (icase)
		cmpFunc = string::iequal;
	else
		cmpFunc = string::equal;

	dodoStringList::const_iterator i(arr.begin()), j(arr.end());
	for (; i != j; ++i)
		if (cmpFunc(*i, needle))
			return true;

	return false;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
misc::explode(const dodoString &fields,
			  escape escapeF,
			  const dodoString &separator,
			  int limit)
{
	unsigned long i(0), j(0), sep_size(separator.size());
	int k(1);
	dodoStringArray arr;

	while (true)
	{
		if (limit != -1)
		{
			if (k > limit)
			{
				arr.back().append(escapeF(fields.substr(j - sep_size)));

				break;
			}
			++k;
		}

		i = fields.find(separator, i);
		arr.push_back(escapeF(fields.substr(j, i - j)));
		if (i == dodoString::npos)
			break;

		i += sep_size;
		j = i;
	}

	return arr;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
misc::explode(const dodoString &fields,
			  const dodoString &separator,
			  int limit)
{
	return explode(fields, &dummyEscape, separator, limit);
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  escape escapeF,
			  const dodoString &separator,
			  const dodoString &frame,
			  int limit)
{
	if (fields.size() == 0)
		throw exception::basic(exception::ERRMODULE_TOOLSMISC, MISCEX_IMPLODE, exception::ERRNO_LIBDODO, MISCEX_EMPTYARRAY, TOOLSMISCEX_EMPTYARRAY_STR, __LINE__, __FILE__);

	int k(0);

	dodoString temp, fs(frame + separator);
	dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			if (limit != -1)
			{
				if (k > limit)
					return temp;
				++k;
			}
			temp.append(frame + escapeF(*i) + fs);
		}
		temp.append(frame + escapeF(*i) + frame);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  const dodoString &separator,
			  const dodoString &frame,
			  int limit)
{
	return implode(fields, &dummyEscape, separator, frame, limit);
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  const dodoString &separator,
			  int limit)
{
	return implode(fields, &dummyEscape, separator, limit);
}

//-------------------------------------------------------------------

dodoString
misc::implode(const dodoStringArray &fields,
			  escape escapeF,
			  const dodoString &separator,
			  int limit)
{
	if (fields.size() == 0)
		throw exception::basic(exception::ERRMODULE_TOOLSMISC, MISCEX_IMPLODE, exception::ERRNO_LIBDODO, MISCEX_EMPTYARRAY, TOOLSMISCEX_EMPTYARRAY_STR, __LINE__, __FILE__);

	int k(0);

	dodoString temp;
	dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			if (limit != -1)
			{
				if (k > limit)
					return temp;
				++k;
			}
			temp.append(escapeF(*i) + separator);
		}
		temp.append(escapeF(*i));
	}

	return temp;
}

//-------------------------------------------------------------------
