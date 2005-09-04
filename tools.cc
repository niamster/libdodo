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
 
#include "tools.h"

using namespace dodo;

tools *
tools::getSelf()
{
	return this;
}

//-------------------------------------------------------------------

inline void 
tools::addF(int &flag, 
	unsigned int statement)
{
	flag |= statement;
}

//-------------------------------------------------------------------

inline void 
tools::removeF(int &flag, 
		unsigned int statement)
{
	flag &= ~statement-1;
}

//-------------------------------------------------------------------

void 
tools::replace(pchar needle, 
		pchar replacement,
		std::string &data)
{
	register unsigned int i(0),j(strlen(needle)),k(strlen(replacement)),size(data.size());
	while (i<=size)
	{
		i = data.find(needle,i);
		if (i==std::string::npos)
			break;
		data.replace(i,j,replacement,0,k);
		i += k;
	}
}

//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		escape escapeF, 
		std::string separator,
		std::string frame)
{
	std::string temp, fs(frame + separator);
	stringArr::const_iterator i(fields.begin()), j(fields.end());
	
	for (;i!=j-1;++i)
		temp.append(frame + escapeF(*i) + fs);
	temp.append(frame + escapeF(*i) + frame);
	
	return temp;
}

//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		std::string separator)
{
	std::string temp;
	stringArr::const_iterator i(fields.begin()), j(fields.end());
	
	for (;i!=j-1;++i)
		temp.append(*i + separator);
	temp.append(*i);
	
	return temp;
}

//-------------------------------------------------------------------

std::string 
tools::implode(const stringArr &fields, 
		escape escapeF, 
		std::string separator)
{
	std::string temp;
	stringArr::const_iterator i(fields.begin()), j(fields.end());
	
	for (;i!=j-1;++i)
		temp.append(escapeF(*i) + separator);
	temp.append(escapeF(*i));
	
	return temp;	
}

//-------------------------------------------------------------------

inline std::string 
tools::lToString(long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%d",number);
	return std::string(temp);
}

//-------------------------------------------------------------------

inline std::string 
dToString(double number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%f",number);
	return std::string(temp);	
}

//-------------------------------------------------------------------
