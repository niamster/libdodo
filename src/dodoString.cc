/***************************************************************************
 *            dodoString.cc
 *
 *  Tue Aug 28 21:27:19 2007
 *  Copyright  2007  Ni@m
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


#include <dodoString.h>

using namespace dodo;

dodoString::dodoString()
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const std::string &str) : std::string(str)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const char *str) : std::string(str)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const dodoString &str) : std::string(str)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const char    *str,
					   unsigned long length) : std::string(str, length)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const std::string &str,
					   unsigned long index,
					   unsigned long length) : std::string(str, index, length)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(unsigned long length,
					   const char &ch) : std::string(length, ch)
{
}

//-------------------------------------------------------------------

void
dodoString::format(const dodoString &format, ...)
{
	unsigned long length = format.size() * 3;
	char *str = new char[length];
	va_list ap;

	vsnprintf(str, length, format.c_str(), ap);

	*this = str;

	delete [] str;
}

//-------------------------------------------------------------------

void
dodoString::fromL(long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%ld", number);

	*this = temp;
}

//-------------------------------------------------------------------

void
dodoString::fromUL(unsigned long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%ld", number);

	*this = temp;
}

//-------------------------------------------------------------------

void
dodoString::fromI(int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%d", number);

	*this = temp;
}

//-------------------------------------------------------------------

void
dodoString::fromUI(unsigned int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%u", number);

	*this = temp;
}

//-------------------------------------------------------------------

void
dodoString::fromS(short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%sd", number);

	*this = temp;
}

//-------------------------------------------------------------------

void
dodoString::fromUS(unsigned short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%sd", number);

	*this = temp;
}


//-------------------------------------------------------------------

void
dodoString::fromF(float number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%f", number);

	*this = temp;
}

//-------------------------------------------------------------------

void
dodoString::fromD(double number)
{
	char temp[SIZEOFNUM];
	sprintf(temp, "%f", number);

	*this = temp;
}

//-------------------------------------------------------------------

void
dodoString::lTrim(char symbol)
{
	int size = this->size(), i(0);

	for (; i < size; ++i)
		if ((*this)[i] != symbol)
			break;

	this->substr(i, size - i);
}

//-------------------------------------------------------------------

void
dodoString::rTrim(char symbol)
{
	int i(this->size() - 1);

	for (; i >= 0; --i)
		if ((*this)[i] != symbol)
			break;

	this->substr(0, i + 1);
}

//-------------------------------------------------------------------

void
dodoString::rTrim(const char symbols[],
				  int symCount)
{
	int i(this->size() - 1), j, empty;

	for (; i >= 0; --i)
	{
		for (j = 0, empty = 0; j < symCount; ++j)
			if ((*this)[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}

	this->substr(0, i + 1);
}

//-------------------------------------------------------------------

void
dodoString::lTrim(const char symbols[],
				  int symCount)
{
	int size = this->size(), i(0), empty, j;

	for (; i < size; ++i)
	{
		for (j = 0, empty = 0; j < symCount; ++j)
			if ((*this)[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}

	this->substr(i, size - i);
}

//-------------------------------------------------------------------

void
dodoString::trim(const char symbols[],
				 int symCount)
{
	lTrim(symbols, symCount);
	rTrim(symbols, symCount);
}

//-------------------------------------------------------------------

void
dodoString::trim(char symbol)
{
	lTrim(symbol);
	rTrim(symbol);
}

//-------------------------------------------------------------------

long
dodoString::toL()
{
	return atol(this->c_str());
}

//-------------------------------------------------------------------

unsigned long
dodoString::toUL()
{
	return strtoul(this->c_str(), NULL, 10);
}

//-------------------------------------------------------------------

int
dodoString::toI()
{
	return atoi(this->c_str());
}

//-------------------------------------------------------------------

unsigned int
dodoString::toUI()
{
	return (unsigned int)atol(this->c_str());
}

//-------------------------------------------------------------------

short
dodoString::toS()
{
	return (short)atoi(this->c_str());
}

//-------------------------------------------------------------------

unsigned short
dodoString::toUS()
{
	return (unsigned short)atoi(this->c_str());
}

//-------------------------------------------------------------------

float
dodoString::toF()
{
	return (float)atof(this->c_str());
}

//-------------------------------------------------------------------

double
dodoString::toD()
{
	return atof(this->c_str());
}

//-------------------------------------------------------------------
