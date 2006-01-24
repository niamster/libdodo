/***************************************************************************
 *            cgiProcessor.cc
 *
 *  Sun Jan 22 19:05:57 2006
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <cgiProcessor.h>

using namespace dodo;

cgiProcessor::cgiProcessor()
{
}

//-------------------------------------------------------------------

cgiProcessor::~cgiProcessor()
{
}
//-------------------------------------------------------------------

std::string 
cgiProcessor::process(const std::string &path)
{
	return _process(preProcess(path), path);
}

//-------------------------------------------------------------------

std::string 
cgiProcessor::_process(const std::string &buffer, 
					const std::string &path)
{
	register unsigned long i(0), j(0), begin(0), k(0);
	std::string temp1;
	
	std::string tpl;
	
	while (true)
	{	
		begin = j;
		
		i = buffer.find("<(",begin);
		if (i == std::string::npos)
		{
			tpl.append(buffer.substr(begin));
			break;
		}
		else
			tpl.append(buffer.substr(begin,i - begin));
		
		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find("<)>",i);

			++i;
			tpl.append(buffer.substr(i,j - i));
			j += 3;
			continue;
		}
		
		j = buffer.find(")>",i);
							
		temp = buffer.substr(i, j - i);
		
		j += 2;
		
		k = temp.find("include");	
		if (k != std::string::npos)
		{
			temp1 = tools::trim(temp.substr(k + 8)," \t\n\"'",5);

			if (temp1[0] == '$')
				temp1 = getVar(temp1.substr(1));
			
			if (strcmp(temp1.c_str(),path.c_str()) != 0 && !recursive(temp1))
			{
				processed.push_back(path);
				tpl.append(process(temp1));
				processed.pop_back();
			}
		}				
		else
			tpl.append(buffer.substr(i - 2,j - i + 2));
	}
	
	return tpl;
}

//-------------------------------------------------------------------

bool 
cgiProcessor::recursive(const std::string &path)
{
	i = processed.begin();
	j = processed.end();
	
	for (;i!=j;++i)
		if (strcmp(i->c_str(),path.c_str()) == 0)
			return true;
			
	return false;		
}

//-------------------------------------------------------------------

void 
cgiProcessor::assign(const std::string &varName, 
						const std::string &varVal)
{
	global[varName] = varVal;
}

//-------------------------------------------------------------------

std::string 
cgiProcessor::getVar(const std::string &varName)
{
	k = local.begin();
	l = local.end();
	
	for (;k!=l;++k)
		if (strcmp(varName.c_str(),k->first.c_str()) == 0)
			return k->second;

	k = global.begin();
	l = global.end();
	
	for (;k!=l;++k)
		if (strcmp(varName.c_str(),k->first.c_str()) == 0)
			return k->second;
			
	return __string__;
}

//-------------------------------------------------------------------
