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

dodoString::dodoString(const char *str, 
						unsigned long  length) : std::string(str, length)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(const std::string &str, 
						unsigned long  index, 
						unsigned long  length) : std::string(str, index, length)
{
}

//-------------------------------------------------------------------

dodoString::dodoString(unsigned long  length, 
						const char &ch) : std::string(length, ch)
{
}

//-------------------------------------------------------------------

bool 
dodoString::equal(const dodoString &first, 
					const dodoString &second)
{
	return strcmp(first.c_str(), second.c_str()) == 0?true:false;
}

//-------------------------------------------------------------------

bool 
dodoString::iequal(const dodoString &first, 
					const dodoString &second)
{
	return strcasecmp(first.c_str(), second.c_str()) == 0?true:false;
}

//-------------------------------------------------------------------

dodoString 
dodoString::format(const dodoString &format, ...)
{
	unsigned long length = format.size() * 3;
	char *str = new char[length];
	va_list ap;
	
	vsnprintf(str, length, format.c_str(), ap);
	
	std::string res = str;
	
	delete [] str;
	
	return res;
}

//-------------------------------------------------------------------

dodoString 
dodoString::lToString(long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%ld", number);
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::ulToString(unsigned long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%ld", number);
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::iToString(int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%d", number);
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::uiToString(unsigned int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%u", number);
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::sToString(short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%sd", number);
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::usToString(unsigned short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%sd", number);
	
	return temp;
}


//-------------------------------------------------------------------

dodoString 
dodoString::fToString(float number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%f", number);
	
	return temp;	
}

//-------------------------------------------------------------------

dodoString 
dodoString::dToString(double number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%f", number);
	
	return temp;	
}

//-------------------------------------------------------------------

dodoString 
dodoString::fromL(long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%ld", number);

	*this = temp;
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::fromUL(unsigned long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%ld", number);

	*this = temp;
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::fromI(int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%d", number);

	*this = temp;
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::fromUI(unsigned int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%u", number);

	*this = temp;
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::fromS(short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%sd", number);

	*this = temp;
	
	return temp;
}

//-------------------------------------------------------------------

dodoString 
dodoString::fromUS(unsigned short number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%sd", number);

	*this = temp;
	
	return temp;
}


//-------------------------------------------------------------------

dodoString 
dodoString::fromF(float number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%f", number);

	*this = temp;
	
	return temp;	
}

//-------------------------------------------------------------------

dodoString 
dodoString::fromD(double number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%f", number);

	*this = temp;
	
	return temp;	
}

//-------------------------------------------------------------------

dodoString 
dodoString::lTrim(const dodoString &data, 
			char symbol)
{
	int size = data.size(), i(0);
	
	for (;i<size;++i)
		if (data[i] != symbol)
			break;
	
	return data.substr(i, size-i);		
}

//-------------------------------------------------------------------

dodoString 
dodoString::rTrim(const dodoString &data, 
			char symbol)
{
	int i(data.size()-1);
	
	for (;i>=0;--i)
		if (data[i] != symbol)
			break;
	
	return data.substr(0, i+1);	
}

//-------------------------------------------------------------------

dodoString 
dodoString::rTrim(const dodoString &data, 
			const char symbols[], 
			int symCount)
{
	int i(data.size()-1), j, empty;
	
	for (;i>=0;--i)
	{
		for (j=0, empty=0;j<symCount;++j)
			if (data[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}
	
	return data.substr(0, i+1);
}

//-------------------------------------------------------------------

dodoString 
dodoString::lTrim(const dodoString &data, 
			const char symbols[], 
			int symCount)
{
	int size = data.size(), i(0), empty, j;
	
	for (;i<size;++i)
	{
		for (j=0, empty=0;j<symCount;++j)
			if (data[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}
	
	return data.substr(i, size-i);		
}

//-------------------------------------------------------------------

dodoString 
dodoString::trim(const dodoString &data, 
			const char symbols[], 
			int symCount)
{
	return rTrim(lTrim(data, symbols, symCount), symbols, symCount);
}

//-------------------------------------------------------------------

dodoString 
dodoString::trim(const dodoString &data, 
			char symbol)
{
	return rTrim(lTrim(data, symbol), symbol);
}

//-------------------------------------------------------------------

dodoString 
dodoString::lTrim(char symbol)
{
	int size = this->size(), i(0);
	
	for (;i<size;++i)
		if ((*this)[i] != symbol)
			break;
	
	return this->substr(i, size-i);		
}

//-------------------------------------------------------------------

dodoString 
dodoString::rTrim(char symbol)
{
	int i(this->size()-1);
	
	for (;i>=0;--i)
		if ((*this)[i] != symbol)
			break;
	
	return this->substr(0, i+1);	
}

//-------------------------------------------------------------------

dodoString 
dodoString::rTrim(const char symbols[], 
					int symCount)
{
	int i(this->size()-1), j, empty;
	
	for (;i>=0;--i)
	{
		for (j=0, empty=0;j<symCount;++j)
			if ((*this)[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}
	
	return this->substr(0, i+1);
}

//-------------------------------------------------------------------

dodoString 
dodoString::lTrim(const char symbols[], 
			int symCount)
{
	int size = this->size(), i(0), empty, j;
	
	for (;i<size;++i)
	{
		for (j=0, empty=0;j<symCount;++j)
			if ((*this)[i] != symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}
	
	return this->substr(i, size-i);		
}

//-------------------------------------------------------------------

dodoString 
dodoString::trim(const char symbols[], 
			int symCount)
{
	lTrim(symbols, symCount);
	return rTrim(symbols, symCount);
}

//-------------------------------------------------------------------

dodoString 
dodoString::trim(char symbol)
{
	lTrim(symbol);
	return rTrim(symbol);
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

long 
dodoString::stringToL(const dodoString &data)
{
	return atol(data.c_str());
}

//-------------------------------------------------------------------

unsigned long 
dodoString::stringToUL(const dodoString &data)
{
	return strtoul(data.c_str(), NULL, 10);
}

//-------------------------------------------------------------------

int 
dodoString::stringToI(const dodoString &data)
{
	return atoi(data.c_str());
}

//-------------------------------------------------------------------

unsigned int 
dodoString::stringToUI(const dodoString &data)
{
	return (unsigned int)atol(data.c_str());
}

//-------------------------------------------------------------------

short 
dodoString::stringToS(const dodoString &data)
{
	return (short)atoi(data.c_str());
}

//-------------------------------------------------------------------

unsigned short 
dodoString::stringToUS(const dodoString &data)
{
	return (unsigned short)atoi(data.c_str());
}

//-------------------------------------------------------------------

float 
dodoString::stringToF(const dodoString &data)
{
	return (float)atof(data.c_str());
}

//-------------------------------------------------------------------

double 
dodoString::stringToD(const dodoString &data)
{
	return atof(data.c_str());
}


//-------------------------------------------------------------------
