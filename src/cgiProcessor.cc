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
			_include(temp.substr(k + 8),tpl,path);
		}				
		else
		{
			k = temp.find("if");
			if (k != std::string::npos)
			{
				j = _if(buffer,j,temp.substr(k + 2),tpl,path);
			}
			else
				tpl.append(buffer.substr(i - 2,j - i + 2));
		}
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

unsigned long 
cgiProcessor::_if(const std::string &buffer,
				unsigned long start,
				const std::string &statement,
				std::string &tpl, 
				const std::string &path)
{
	register bool _float(false), invert(false);
	
	register unsigned short oper(0);
	
	std::vector<std::string> temp2 = tools::explode(statement,"==");
	if (temp2.size() != 2)
	{
		temp2 = tools::explode(statement,"!=");
		if (temp2.size() != 2)
		{
			temp2 = tools::explode(statement,"<=");
			if (temp2.size() != 2)
			{
				temp2 = tools::explode(statement,">=");
				if (temp2.size() != 2)
				{
					temp2 = tools::explode(statement,"<");
					if (temp2.size() != 2)
					{
						temp2 = tools::explode(statement,">");
						if (temp2.size() == 2)
						{
							oper = 4;
							_float = true;
						}
					}
					else
					{
						oper = 3;
						_float = true;
					}
				}
				else
				{
					oper = 2;
					_float = true;				
				}
			}
			else
			{
				oper = 1;
				_float = true;
			}
		}
		else
			invert = true;			
	}
	
	register bool accept(invert);
	
	if (temp2.size() != 2)
	{
		if (temp2.size() != 1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_WRONGIFSTATEMENT,CGIPREPROCESSOR_WRONGIFSTATEMENT_STR,__LINE__,__FILE__);
			#else
				return tpl;
			#endif
		
		temp1 = tools::trim(temp2[0]," \t\n",3);
		
		if (temp1[0] == '!')
		{
			invert = true;
			temp1 = tools::trim(temp1.substr(1)," \t\n\"'`",6);
		}		
		if (temp1[0] == '$')
			temp1 = getVar(temp1.substr(1));
		
		if (strcmp(temp1.c_str(),"false") != 0 && temp1.size() != 0)
			accept = !invert;
		else
			accept = invert;
	}
	else
	{
		temp1 = tools::trim(temp2[0]," \t\n\"'`",6);
		
		if (temp1[0] == '$')
			temp1 = getVar(temp1.substr(1));
		
		std::string temp3 = tools::trim(temp2[1]," \t\n\"'`",6);
		
		if (temp2[1][0] == '$')
			temp3 = getVar(temp3.substr(1));
		
		if (_float)	
		{
			register double first(atof(temp1.c_str())), second(atof(temp3.c_str()));
			
			switch (oper)
			{
				case 1:
					accept = (first <= second);
					break;
					
				case 2:
					accept = (first >= second);
					break;
					
				case 3:
					accept = (first < second);
					break;
					
				case 4:
					accept = (first > second);
					break;
			}
		}
		else
		{
			if (strcmp(temp1.c_str(),temp3.c_str()) == 0)
				accept = !invert;
		}
	}

	register unsigned long u(blockEnd(buffer,start,"if","fi")), v(0);
	register bool found(true);
	
	try
	{
		v = blockEnd(buffer,start,"if","else");
	}
	catch(...)
	{
		found = false;
	}
	
	if (accept)
	{
		if (!found)
			v = u;
		
		tpl.append(_process(buffer.substr(start,v - start),path));
	}
	else
	{	
		if (found)
		{
			v = buffer.find(")>",v) + 2;
			tpl.append(_process(buffer.substr(v,u - v),path));
		}
	}
	
	return buffer.find(")>",u) + 2;	
}

//-------------------------------------------------------------------

unsigned long 
cgiProcessor::blockEnd(const std::string &buffer, 
						unsigned long start, 
						const std::string &st, 
						const std::string &ts)
{
	register unsigned long u, m(start), _st(1), b, p, stLen(st.size()), tsLen(ts.size());

	while (true)
	{
		u = buffer.find("<(",m);
		if (u == std::string::npos)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_WRONGIFSTATEMENT,CGIPREPROCESSOR_WRONGIFSTATEMENT_STR,__LINE__,__FILE__);
			#else
				return tpl;
			#endif
			
		b = buffer.find(")>",u + 2);
		
		for (p=u;p<b;++p)
			if (buffer[p] != ' ' && buffer[p] != '\t' && buffer[p] != '\n')
			{
				if (strcmp(buffer.substr(p,tsLen).c_str(),ts.c_str()) == 0)
					--_st;
				else
				{
					if (strcmp(buffer.substr(p,stLen).c_str(),st.c_str()) == 0)
						++_st;
				}
			}
		
		if (_st == 0)	
			break;
		
		m = b + 2;	
	}
	
	return u;	
}

//-------------------------------------------------------------------

void 
cgiProcessor::_include(const std::string &statement, 
						std::string &tpl, 
						const std::string &path)
{
	temp1 = tools::trim(statement," \t\n\"'",5);

	if (temp1[0] == '$')
		temp1 = getVar(temp1.substr(1));
	
	if (strcmp(temp1.c_str(),path.c_str()) != 0 && !recursive(temp1))
	{
		processed.push_back(path);
		tpl.append(process(temp1));
		processed.pop_back();
	}	
}						

//-------------------------------------------------------------------
