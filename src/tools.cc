/***************************************************************************
 *            tools.cc
 *
 *  Wed Aug 24 00:43:21 2005
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
 
#include <tools.h>

using namespace dodo;

inline std::string
tools::dummy(const std::string &data)
{
	return data;
}

//-------------------------------------------------------------------

void 
tools::replace(pchar needle, 
		pchar replacement,
		std::string &data)
{
	register unsigned int i(0),j(strlen(needle)),k(strlen(replacement));
	while (true)
	{
		i = data.find(needle,i);
		if (i==std::string::npos)
			break;
		data.replace(i,j,replacement,0,k);
		i += k;
	}
}

//-------------------------------------------------------------------

stringArr 
tools::explode(const std::string &fields,
			escape escapeF,
			const std::string &separator)
{
	register unsigned int i(0), j(0), sep_size(separator.size());
	stringArr arr;
	while (true)
	{
		i = fields.find(separator,i);
		arr.push_back(escapeF(fields.substr(j,i-j)));
		if (i==std::string::npos)
			break;
		i += sep_size;
		j = i;
	}
	return arr;
}

//-------------------------------------------------------------------

stringArr 
tools::explode(const std::string &fields, 
			const std::string &separator)
{
	return explode(fields,&dummy,separator);
}

//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		escape escapeF, 
		const std::string &separator,
		const std::string &frame)
{
	std::string temp, fs(frame + separator);
	stringArr::const_iterator i(fields.begin()), j(fields.end()-1);
	
	for (;i!=j;++i)
		temp.append(frame + escapeF(*i) + fs);
	temp.append(frame + escapeF(*i) + frame);
	
	return temp;
}
//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		const std::string &separator,
		const std::string &frame)
{	
	return implode(fields,&dummy,separator,frame);
}

//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		const std::string &separator)
{
	return implode(fields,&dummy,separator);
}

//-------------------------------------------------------------------

std::string 
tools::implode(const stringArr &fields, 
		escape escapeF, 
		const std::string &separator)
{
	std::string temp;
	stringArr::const_iterator i(fields.begin()), j(fields.end()-1);
	
	for (;i!=j;++i)
		temp.append(escapeF(*i) + separator);
	temp.append(escapeF(*i));
	
	return temp;	
}

//-------------------------------------------------------------------

std::string 
tools::lToString(long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%ld",number);
	return std::string(temp);
}

//-------------------------------------------------------------------

std::string 
tools::dToString(double number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%f",number);
	return std::string(temp);	
}

//-------------------------------------------------------------------

std::string 
tools::lTrim(const std::string &data, 
			char symbol)
{
	register int size = data.size(), i(0);
	
	for (;i<size;++i)
		if (data[i]!=symbol)
			break;
	
	return data.substr(i,size-i);		
}

//-------------------------------------------------------------------

std::string 
tools::rTrim(const std::string &data, 
			char symbol)
{
	register int i(data.size());
	
	for (;i>0;--i)
		if (data[i]!=symbol)
			break;
	
	return data.substr(0,i);	
}

//-------------------------------------------------------------------
