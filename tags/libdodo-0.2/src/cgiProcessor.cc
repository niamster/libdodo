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
			{
				k = temp.find("print");
				if (k != std::string::npos)
				{
					_print(temp.substr(k + 5),tpl);
				}
				else
				{
					k = temp.find("for");
					if(k != std::string::npos)
					{
						j = _for(buffer,j,temp.substr(k + 3),tpl,path);
					}
					else
						tpl.append(buffer.substr(i - 2,j - i + 2));
				}
			}
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
					const std::vector<assocArr> &varVal)
{
	global.erase(varName);
	globalArray.erase(varName);
	globalHash.erase(varName);
	
	globalArrayHash[varName] = varVal;
}					

//-------------------------------------------------------------------

void 
cgiProcessor::assign(const std::string &varName, 
					const assocArr &varVal)
{
	global.erase(varName);
	globalArray.erase(varName);
	globalArrayHash.erase(varName);
	
	globalHash[varName] = varVal;
}					

//-------------------------------------------------------------------

void 
cgiProcessor::assign(const std::string &varName, 
					const stringArr &varVal)
{
	global.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);
	
	globalArray[varName] = varVal;
}					

//-------------------------------------------------------------------

void 
cgiProcessor::assign(const std::string &varName, 
						const std::string &varVal)
{
	globalArray.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);
	
	global[varName] = varVal;
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
				throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__IF,ERR_LIBDODO,CGIPREPROCESSOR_WRONGIFSTATEMENT,CGIPREPROCESSOR_WRONGIFSTATEMENT_STR,__LINE__,__FILE__);
			#else
				return start;
			#endif
		
		temp1 = tools::trim(temp2[0]," \t\n",3);
		
		if (temp1[0] == '!')
		{
			invert = true;
			temp1 = temp1.substr(1);
		}
				
		temp1 = getVar(temp1);
		
		if (strcmp(temp1.c_str(),"false") != 0 && temp1.size() != 0)
			accept = !invert;
		else
			accept = invert;
	}
	else
	{
		temp1 = getVar(temp2[0]);
		
		std::string temp3 = getVar(temp2[1]);
		
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
				throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_BLOCKEND,ERR_LIBDODO,CGIPREPROCESSOR_WRONGIFSTATEMENT,CGIPREPROCESSOR_WRONGIFSTATEMENT_STR,__LINE__,__FILE__);
			#else
				return start;
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
	temp1 = getVar(statement);
	
	if (strcmp(temp1.c_str(),path.c_str()) != 0 && !recursive(temp1))
	{
		processed.push_back(path);
		tpl.append(process(temp1));
		processed.pop_back();
	}	
}						

//-------------------------------------------------------------------

void 
cgiProcessor::_print(const std::string &statement, 
					std::string &tpl)
{
	tpl.append(getVar(statement));
}

//-------------------------------------------------------------------

unsigned long 
cgiProcessor::_for(const std::string &buffer,
				unsigned long start,
				const std::string &statement,
				std::string &tpl, 
				const std::string &path)
{
	unsigned long u(blockEnd(buffer,start,"for","rof"));	
	
	std::string processBuffer = buffer.substr(start,u - start);
	
	unsigned long p = statement.find("$");
	unsigned long i(p), j(statement.size());
	
	for (;i<j;++i)
		if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
			break;
			
	std::string temp1 = statement.substr(p,i - p).substr(1);

	p = statement.find("in",i + 1);
	if (p == std::string::npos)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__FOR,ERR_LIBDODO,CGIPREPROCESSOR_WRONGFORSTATEMENT,CGIPREPROCESSOR_WRONGFORSTATEMENT_STR,__LINE__,__FILE__);
		#else
			return u;
		#endif
	
	std::string temp2 = trim(statement.substr(p + 2));
	std::string temp3 = buffer.substr(start,u - start);
	
	u = buffer.find(")>",u) + 2;
					
	if (temp2[0] == '$')
	{
		temp2 = temp2.substr(1);
		
		stringArr temp = tools::explode(temp2,".");
		
		if (temp.size() == 1)
		{
			assocArr::iterator k = local.begin();
			assocArr::iterator l = local.end();		
			for (;k!=l;++k)
				if (strcmp(temp2.c_str(),k->first.c_str()) == 0)
				{
					assocArr::iterator iter = local.find(temp1);
					std::string iterVal;
					if (iter != local.end())
						iterVal = local[temp1];
					
					register unsigned long i(0),j(k->second.size());
					for (;i<j;++i)
					{
						local[temp1] = std::string(1,k->second[i]);
						tpl.append(_process(temp3,path));
					}
					
					if (iter != local.end())
						local[temp1] = iterVal;
					else
						local.erase(temp1);
					
					return u;
				}

			std::map<std::string, assocArr>::iterator g = localHash.begin();
			std::map<std::string, assocArr>::iterator h = localHash.end();		
			for (;g!=h;++g)
				if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
				{
					assocArr::iterator iter = local.find(temp1);
					std::string iterVal;
					if (iter != local.end())
						iterVal = local[temp1];					
					
					assocArr::iterator k = g->second.begin();
					assocArr::iterator l = g->second.end();			
					for (;k!=l;++k)
					{
						local[temp1] = k->second;
						tpl.append(_process(temp3,path));				
					}
					
					if (iter != local.end())
						local[temp1] = iterVal;
					else
						local.erase(temp1);
					
					return u;
				}
		
			k = global.begin();
			l = global.end();		
			for (;k!=l;++k)
				if (strcmp(temp2.c_str(),k->first.c_str()) == 0)
				{
					assocArr::iterator iter = local.find(temp1);
					std::string iterVal;
					if (iter != local.end())
						iterVal = local[temp1];	
											
					register unsigned long i(0),j(k->second.size());
					for (;i<j;++i)
					{
						local[temp1] = std::string(1,k->second[i]);
						tpl.append(_process(temp3,path));
					}
					
					if (iter != local.end())
						local[temp1] = iterVal;
					else
						local.erase(temp1);
					
					return u;
				}

			g = globalHash.begin();
			h = globalHash.end();		
			for (;g!=h;++g)
				if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
				{
					assocArr::iterator iter = local.find(temp1);
					std::string iterVal;
					if (iter != local.end())
						iterVal = local[temp1];	
											
					assocArr::iterator k = g->second.begin();
					assocArr::iterator l = g->second.end();			
					for (;k!=l;++k)
					{
						local[temp1] = k->second;
						tpl.append(_process(temp3,path));
					}
					
					if (iter != local.end())
						local[temp1] = iterVal;
					else
						local.erase(temp1);
					
					return u;
				}
			
			std::map<std::string, stringArr>::iterator o = globalArray.begin();
			std::map<std::string, stringArr>::iterator p = globalArray.end();		
			for (;o!=p;++o)
				if (strcmp(temp[0].c_str(),o->first.c_str()) == 0)
				{
					assocArr::iterator iter = local.find(temp1);
					std::string iterVal;
					if (iter != local.end())
						iterVal = local[temp1];	
											
					stringArr::iterator k = o->second.begin();
					stringArr::iterator l = o->second.end();			
					for (;k!=l;++k)
					{
						local[temp1] = *k;
						tpl.append(_process(temp3,path));				
					}
					
					if (iter != local.end())
						local[temp1] = iterVal;
					else
						local.erase(temp1);
					
					return u;
				}
				
			std::map<std::string, std::vector<assocArr> >::iterator d = globalArrayHash.begin();
			std::map<std::string, std::vector<assocArr> >::iterator f = globalArrayHash.end();		
			for (;d!=f;++d)
				if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
				{
					std::map<std::string, assocArr>::iterator iter = localHash.find(temp1);
					assocArr iterVal;
					if (iter != localHash.end())
						iterVal = localHash[temp1];					
					
					std::vector<assocArr>::iterator k = d->second.begin();
					std::vector<assocArr>::iterator l = d->second.end();			
					for (;k!=l;++k)
					{
						localHash[temp1] = *k;
						tpl.append(_process(temp3,path));				
					}
					
					if (iter != localHash.end())
						localHash[temp1] = iterVal;
					else
						localHash.erase(temp1);
					
					return u;
				}												
		}
		else
		{
			if (temp.size() == 2)
			{
				std::map<std::string, assocArr>::iterator g = localHash.begin();
				std::map<std::string, assocArr>::iterator h = localHash.end();		
				for (;g!=h;++g)
					if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
					{							
						assocArr::iterator k = g->second.begin();
						assocArr::iterator l = g->second.end();			
						for (;k!=l;++k)
							if (strcmp(temp[1].c_str(),k->first.c_str()) == 0)
							{
								assocArr::iterator iter = local.find(temp1);
								std::string iterVal;
								if (iter != local.end())
									iterVal = local[temp1];	
																	
								register unsigned long i(0),j(k->second.size());
								for (;i<j;++i)
								{
									local[temp1] = std::string(1,k->second[i]);
									tpl.append(_process(temp3,path));
								}
					
								if (iter != local.end())
									local[temp1] = iterVal;
								else
									local.erase(temp1);
								
								return u;
							}
					}
							
				g = globalHash.begin();
				h = globalHash.end();		
				for (;g!=h;++g)
					if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
					{								
						assocArr::iterator k = g->second.begin();
						assocArr::iterator l = g->second.end();			
						for (;k!=l;++k)
							if (strcmp(temp[1].c_str(),k->first.c_str()) == 0)
							{
								assocArr::iterator iter = local.find(temp1);
								std::string iterVal;
								if (iter != local.end())
									iterVal = local[temp1];	
																	
								register unsigned long i(0),j(k->second.size());
								for (;i<j;++i)
								{
									local[temp1] = std::string(1,k->second[i]);
									tpl.append(_process(temp3,path));
								}
								
								if (iter != local.end())
									local[temp1] = iterVal;
								else
									local.erase(temp1);
								
								return u;
							}
					}
					
				std::map<std::string, stringArr>::iterator o = globalArray.begin();
				std::map<std::string, stringArr>::iterator p = globalArray.end();		
				for (;o!=p;++o)
					if (strcmp(temp[0].c_str(),o->first.c_str()) == 0)
					{
						register unsigned long pos = atol(temp[1].c_str());
						if (pos >= 0 && pos <= o->second.size())
						{
							assocArr::iterator iter = local.find(temp1);
							std::string iterVal;
							if (iter != local.end())
								iterVal = local[temp1];	
																
							register unsigned long i(0),j(o->second[pos].size());
							for (;i<j;++i)
							{
								local[temp1] = std::string(1,o->second[pos][i]);
								tpl.append(_process(temp3,path));
							}
							
							if (iter != local.end())
								local[temp1] = iterVal;
							else
								local.erase(temp1);
							
							return u;
						}
					}							
	
				std::map<std::string, std::vector<assocArr> >::iterator d = globalArrayHash.begin();
				std::map<std::string, std::vector<assocArr> >::iterator f = globalArrayHash.end();
				for (;d!=f;++d)
					if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
					{
						register unsigned long pos = atol(temp[1].c_str());
						if (pos >= 0 && pos <= d->second.size())
						{
							assocArr::iterator iter = local.find(temp1);
							std::string iterVal;
							if (iter != local.end())
								iterVal = local[temp1];	
																
							assocArr::iterator k = d->second[pos].begin();					
							assocArr::iterator l = d->second[pos].end();
							for (;k!=l;++k)
							{
									local[temp1] = k->second;
									tpl.append(_process(temp3,path));		
							}
							
							if (iter != local.end())
								local[temp1] = iterVal;
							else
								local.erase(temp1);
							
							return u;
						}
					}
			}
			else	
			{
				if (temp.size() == 3)
				{
					std::map<std::string, std::vector<assocArr> >::iterator d = globalArrayHash.begin();
					std::map<std::string, std::vector<assocArr> >::iterator f = globalArrayHash.end();
					for (;d!=f;++d)
						if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
						{
							register unsigned long pos = atol(temp[1].c_str());
							if (pos >= 0 && pos <= d->second.size())
							{
								assocArr::iterator k = d->second[pos].begin();					
								assocArr::iterator l = d->second[pos].end();
								for (;k!=l;++k)
									if (strcmp(temp[2].c_str(),k->first.c_str()) == 0)
									{
										assocArr::iterator iter = local.find(temp1);
										std::string iterVal;
										if (iter != local.end())
											iterVal = local[temp1];	
								
										register unsigned long i(0),j(k->second.size());
										for (;i<j;++i)
										{
											local[temp1] = std::string(1,k->second[i]);
											tpl.append(_process(temp3,path));
										}
							
										if (iter != local.end())
											local[temp1] = iterVal;
										else
											local.erase(temp1);
										
										return u;
									}						
							}
						}
				}
			}
		}		
	}
	else
	{

		assocArr::iterator iter = local.find(temp1);
		std::string iterVal;
		if (iter != local.end())
			iterVal = local[temp1];	
					
		register unsigned long i(0), j(temp2.size());
		for (;i<j;++i)
		{
			local[temp1] = std::string(1,temp2[i]);
			tpl.append(_process(temp3,path));			
		}
		
		if (iter != local.end())
			local[temp1] = iterVal;
		else
			local.erase(temp1);
		
		return u;
	}
	
	return u;	
}					

//-------------------------------------------------------------------

std::string 
cgiProcessor::getVar(const std::string &a_varName)
{
	std::string varName = trim(a_varName), tempVar;
	register unsigned long u, b, m(0), ob, cb, i, c;
	
	while (true)
	{
		ob = 1;
		cb = 0;
		
		u = varName.find("{",m);
		if (u == std::string::npos)
			break;
		
		c = u;
		while (true)	
		{
			b = varName.find("}",c + 1);
			if (b == std::string::npos)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_GETVAR,ERR_LIBDODO,CGIPREPROCESSOR_WRONGVARSTATEMENT,CGIPREPROCESSOR_WRONGVARSTATEMENT_STR,__LINE__,__FILE__);
				#else
					return __string__;
				#endif
			
			++cb;
			
			for (i=c+1;i<b;++i)	
				if (varName[i] == '{')
					++ob;

			c = b;
			
			if (cb == ob)		
				break;
		}
		
		tempVar = trim(varName.substr(u + 1,b - 1 - u));
		if (tempVar[0] == '$')
			tempVar = getVar(tempVar);
		
		varName.replace(u,b + 1 - u,tempVar,0,tempVar.size());
							
		m = b - 1;	
	}	
	
	if (varName[0] != '$')
		return varName;
	
	varName.erase(0,1);
	
	stringArr temp = tools::explode(varName,".");
	
	if (temp.size() == 1)
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
	}
	else
	{
		k = local.begin();
		l = local.end();		
		for (;k!=l;++k)
			if (strcmp(temp[0].c_str(),k->first.c_str()) == 0)
			{
				register unsigned long pos = atol(temp[1].c_str());
				if (pos >= 0 && pos <= k->second.size())
					return std::string(1,k->second[pos]);
			}

		g = localHash.begin();
		h = localHash.end();		
		for (;g!=h;++g)
			if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
			{
				k = g->second.begin();
				l = g->second.end();			
				for (;k!=l;++k)
					if (strcmp(temp[1].c_str(),k->first.c_str()) == 0)
					{
						if (temp.size() == 3)
						{
							register unsigned long pos = atol(temp[2].c_str());
							if (pos >= 0 && pos <= k->second.size())
								return std::string(1,k->second[pos]);
						}
						else
							return k->second;
					}
			}
	
		k = global.begin();
		l = global.end();		
		for (;k!=l;++k)
			if (strcmp(temp[0].c_str(),k->first.c_str()) == 0)
			{
				register unsigned long pos = atol(temp[1].c_str());
				if (pos >= 0 && pos <= k->second.size())
					return std::string(1,k->second[pos]);
			}
					
		g = globalHash.begin();
		h = globalHash.end();		
		for (;g!=h;++g)
			if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
			{
				k = g->second.begin();
				l = g->second.end();			
				for (;k!=l;++k)
					if (strcmp(temp[1].c_str(),k->first.c_str()) == 0)
					{
						if (temp.size() == 3)
						{
							register unsigned long pos = atol(temp[2].c_str());
							if (pos >= 0 && pos <= k->second.size())
								return std::string(1,k->second[pos]);
						}
						else
							return k->second;
					}
			}
			
		o = globalArray.begin();
		p = globalArray.end();		
		for (;o!=p;++o)
			if (strcmp(temp[0].c_str(),o->first.c_str()) == 0)
			{
				register unsigned long pos = atol(temp[1].c_str());
				if (pos >= 0 && pos <= o->second.size())
				{
					if (temp.size() == 3)
					{
						register unsigned long pos1 = atol(temp[2].c_str());
						if (pos >= 0 && pos1 <= o->second[pos].size())
							return std::string(1,o->second[pos][pos1]);
					}
					else
						return o->second[pos];
				}
			}							
		
		if (temp.size() >= 3)	
		{
			d = globalArrayHash.begin();
			f = globalArrayHash.end();
			for (;d!=f;++d)
				if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
				{
					register unsigned long pos = atol(temp[1].c_str());
					if (pos >= 0 && pos <= d->second.size())
					{
						k = d->second[pos].begin();					
						l = d->second[pos].end();
						for (;k!=l;++k)
							if (strcmp(temp[2].c_str(),k->first.c_str()) == 0)
							{
								if (temp.size() == 4)
								{
									pos = atol(temp[3].c_str());
									if (pos >= 0 && pos <= k->second.size())
										return std::string(1,k->second[pos]);
								}
								else
									return k->second;
							}						
					}
				}
		}
	}		
	
	return __string__;
}

//-------------------------------------------------------------------

std::string 
cgiProcessor::trim(const std::string &statement)
{
	temp = tools::trim(statement," \t\n",3);
	
	register unsigned long i(temp.size() - 1);
	
	if (temp[0] == '\"' && temp[i] == '\"')
	{
		temp.erase(i);
		temp.erase(0,1);
	}
	else
	{
		if (temp[0] == '\'' && temp[i] == '\'')
		{
			temp.erase(i);
			temp.erase(0,1);
		}
		else
		{
			if (temp[0] == '`' && temp[i] == '`')
			{
				temp.erase(i);
				temp.erase(0,1);
			}
		}
	}
	
	return temp;
}

//-------------------------------------------------------------------
