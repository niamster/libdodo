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


#include <libdodo/dodoString.h>

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
	*this = stringTools::lToString(number);
}

//-------------------------------------------------------------------

void
dodoString::fromUL(unsigned long number)
{
	*this = stringTools::ulToString(number);
}

//-------------------------------------------------------------------

void
dodoString::fromI(int number)
{
	*this = stringTools::iToString(number);
}

//-------------------------------------------------------------------

void
dodoString::fromUI(unsigned int number)
{
	*this = stringTools::uiToString(number);
}

//-------------------------------------------------------------------

void
dodoString::fromS(short number)
{
	*this = stringTools::sToString(number);
}

//-------------------------------------------------------------------

void
dodoString::fromUS(unsigned short number)
{
	*this = stringTools::usToString(number);
}


//-------------------------------------------------------------------

void
dodoString::fromF(float number)
{
	*this = stringTools::fToString(number);
}

//-------------------------------------------------------------------

void
dodoString::fromD(double number)
{
	*this = stringTools::dToString(number);
}

//-------------------------------------------------------------------

void
dodoString::lTrim(char symbol)
{
	stringTools::lTrim(*this, symbol);
}

//-------------------------------------------------------------------

void
dodoString::rTrim(char symbol)
{
	stringTools::rTrim(*this, symbol);
}

//-------------------------------------------------------------------

void
dodoString::rTrim(const char symbols[],
				  int symCount)
{
	stringTools::rTrim(*this, symbols, symCount);
}

//-------------------------------------------------------------------

void
dodoString::lTrim(const char symbols[],
				  int symCount)
{
	stringTools::lTrim(*this, symbols, symCount);
}

//-------------------------------------------------------------------

void
dodoString::trim(const char symbols[],
				 int symCount)
{
	stringTools::lTrim(*this, symbols, symCount);
	stringTools::rTrim(*this, symbols, symCount);
}

//-------------------------------------------------------------------

void
dodoString::trim(char symbol)
{
	stringTools::lTrim(*this, symbol);
	stringTools::rTrim(*this, symbol);
}

//-------------------------------------------------------------------

long
dodoString::toL()
{
	return stringTools::stringToL(*this);
}

//-------------------------------------------------------------------

unsigned long
dodoString::toUL()
{
	return stringTools::stringToUL(*this);
}

//-------------------------------------------------------------------

int
dodoString::toI()
{
	return stringTools::stringToI(*this);
}

//-------------------------------------------------------------------

unsigned int
dodoString::toUI()
{
	return (unsigned int)stringTools::stringToL(*this);
}

//-------------------------------------------------------------------

short
dodoString::toS()
{
	return (short)stringTools::stringToI(*this);
}

//-------------------------------------------------------------------

unsigned short
dodoString::toUS()
{
	return (unsigned short)stringTools::stringToI(*this);
}

//-------------------------------------------------------------------

float
dodoString::toF()
{
	return (float)stringTools::stringToF(*this);
}

//-------------------------------------------------------------------

double
dodoString::toD()
{
	return stringTools::stringToD(*this);
}

//-------------------------------------------------------------------
