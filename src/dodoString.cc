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
dodoString::iToString(int number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%d", number);
	
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

