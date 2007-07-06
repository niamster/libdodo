/***************************************************************************
 *            cgiProcessor.cc
 *
 *  Sun Jan 22 19:05:57 2006
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

#include <cgiProcessor.h>

using namespace dodo;

cgiProcessor::cgiProcessor() : _continueFlag(false),
							_breakDeepness(0),
							_loopDeepness(0),
							iterator(1),
							namespaceDeepness(1)
							#ifdef FCGI_EXT
								,
								cgiFastSet(false)
							#endif	
{
	dodo["version"] = PACKAGE_STRING;
	dodo["iterator"] = "1";

	fstd = new flushSTD;
}
//-------------------------------------------------------------------

#ifdef FCGI_EXT

	cgiProcessor::cgiProcessor(cgiFastSTD *a_cf) : _continueFlag(false),
													_breakDeepness(0),
													_loopDeepness(0),
													iterator(1),
													namespaceDeepness(1),
													cgiFastSet(true),
													cf(a_cf)
					
	{
		dodo["version"] = PACKAGE_STRING;
		dodo["iterator"] = "1";
	}

#endif	

//-------------------------------------------------------------------

cgiProcessor::~cgiProcessor()
{
	if (!cgiFastSet)
		delete fstd;
}

//-------------------------------------------------------------------

dodoString 
cgiProcessor::process(const dodoString &path)
{	
	std::string tmp = _process(preProcess(path), path);

	newLinePositions.pop_back();
	
	return tmp;
}

//-------------------------------------------------------------------

void 
cgiProcessor::clear()
{
	globalArray.clear();
	globalHash.clear();
	globalArrayHash.clear();
	global.clear();
	
	processed.clear();
	localHash.clear();
	local.clear();
	localNamespace.clear();
	namespaceVars.clear();
}

//-------------------------------------------------------------------

dodoString 
cgiProcessor::_process(const dodoString &buffer, 
					const dodoString &path)
{
	unsigned long i(0), j(0), begin(0), k(0);
	
	dodoString tpl;
	dodoString temp;
	
	while (true)
	{	
		begin = j;
		
		i = buffer.find("<(",begin);
		if (i == dodoString::npos)
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

		if (buffer[i] == '*')
		{
			j = buffer.find("*)>",i);

			j += 3;
			
			continue;
		}
		
		j = buffer.find(")>",i);
							
		temp = buffer.substr(i, j - i);
		
		j += 2;
	
		k = temp.find("include");
		if (k != dodoString::npos)
		{
			_include(temp.substr(k + 8),tpl,j,path);
		}				
		else
		{
			k = temp.find("if");
			if (k != dodoString::npos)
			{
				++namespaceDeepness;
				
				j = _if(buffer,j,temp.substr(k + 2),tpl,path);
				
				cleanNamespace();
				
				--namespaceDeepness;
			}
			else
			{
				k = temp.find("print");
				if (k != dodoString::npos)
				{
					_print(temp.substr(k + 5),tpl,j,path);
				}
				else
				{
					k = temp.find("for");
					if(k != dodoString::npos)
					{
						++_loopDeepness;
						++namespaceDeepness;
						
						j = _for(buffer,j,temp.substr(k + 3),tpl,path);
						
						cleanNamespace();
						
						--namespaceDeepness;
						--_loopDeepness;
					}
					else
					{
						k = temp.find("break");
						if(k != dodoString::npos)
						{
							if (_break(temp.substr(k + 5),j,path))
								break;
						}
						else
						{
							k = temp.find("continue");
							if(k != dodoString::npos)
							{
								if (_loopDeepness > 0)
								{
									_continueFlag = true;
									break;
								}
							}
							else
							{
								k = temp.find("assign");
								if (k != dodoString::npos)
								{
									_assign(temp.substr(k + 6),j,path);
								}
								else
								{
									k = temp.find("ns");
									if (k != dodoString::npos)
									{
										++namespaceDeepness;
										
										j = _ns(buffer,j,tpl,path);
										
										cleanNamespace();
										
										--namespaceDeepness;
									}
									else
										tpl.append(buffer.substr(i - 2,j - i + 2));
								}
							}
						}
					}
				}
			}
		}
		
		if (_breakDeepness > 0)
			break;
		
		if (_continueFlag)
			break;	
	}
	
	return tpl;
}

//-------------------------------------------------------------------

bool 
cgiProcessor::recursive(const dodoString &path)
{
	std::list<dodoString>::iterator i(processed.begin()), j(processed.end());
	for (;i!=j;++i)
		if (strcmp(i->c_str(),path.c_str()) == 0)
			return true;
			
	return false;		
}

//-------------------------------------------------------------------

void 
cgiProcessor::assign(dodoString varName, 
					const dodoArray<dodoAssocArr> &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);
		
	if (strcmp(varName.c_str(),"dodo") == 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_ASSIGN,ERR_LIBDODO,CGIPREPROCESSOR_DODOISRESERVEDVARNAME,CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR,__LINE__,__FILE__);
		#else
			return ;
		#endif
	
	global.erase(varName);
	globalArray.erase(varName);
	globalHash.erase(varName);
	
	globalArrayHash[varName] = varVal;
}					

//-------------------------------------------------------------------

void 
cgiProcessor::assign(dodoString varName, 
					const dodoAssocArr &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);
		
	if (strcmp(varName.c_str(),"dodo") == 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_ASSIGN,ERR_LIBDODO,CGIPREPROCESSOR_DODOISRESERVEDVARNAME,CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR,__LINE__,__FILE__);
		#else
			return ;
		#endif
		
	global.erase(varName);
	globalArray.erase(varName);
	globalArrayHash.erase(varName);
	
	globalHash[varName] = varVal;
}					

//-------------------------------------------------------------------

void 
cgiProcessor::assign(dodoString varName, 
					const dodoStringArr &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);
		
	if (strcmp(varName.c_str(),"dodo") == 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_ASSIGN,ERR_LIBDODO,CGIPREPROCESSOR_DODOISRESERVEDVARNAME,CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR,__LINE__,__FILE__);
		#else
			return ;
		#endif	
		
	global.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);
	
	globalArray[varName] = varVal;
}					

//-------------------------------------------------------------------

void 
cgiProcessor::assign(dodoString varName, 
						const dodoString &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);
		
	if (strcmp(varName.c_str(),"dodo") == 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_ASSIGN,ERR_LIBDODO,CGIPREPROCESSOR_DODOISRESERVEDVARNAME,CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR,__LINE__,__FILE__);
		#else
			return ;
		#endif	
		
	globalArray.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);
	
	global[varName] = varVal;
}

//-------------------------------------------------------------------

unsigned long 
cgiProcessor::_if(const dodoString &buffer,
				unsigned long start,
				const dodoString &statement,
				dodoString &tpl, 
				const dodoString &path)
{
	bool _float(false), invert(false);
	
	unsigned short oper(0);
	
	dodoArray<dodoString> temp2 = tools::explode(statement,"==");
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
	
	bool accept(invert);
	
	if (temp2.size() != 2)
	{
		if (temp2.size() != 1)
			#ifndef NO_EX
			{
				char message[128];
				sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
				throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__IF,ERR_LIBDODO,CGIPREPROCESSOR_WRONGIFSTATEMENT,CGIPREPROCESSOR_WRONGIFSTATEMENT_STR,__LINE__,__FILE__,message);
			}
			#else
				return start;
			#endif	
		
		dodoString temp1 = tools::trim(temp2[0]," \t\n",3);
		
		if (temp1[0] == '!')
		{
			invert = true;
			temp1 = temp1.substr(1);
		}
				
		temp1 = getVar(temp1,start,path);
		
		if (strcmp(temp1.c_str(),"false") != 0 && temp1.size() != 0)
			accept = !invert;
		else
			accept = invert;
	}
	else
	{
		dodoString temp1 = getVar(temp2[0],start,path);
		
		dodoString temp3 = getVar(temp2[1],start,path);
		
		if (_float)	
		{
			double first(atof(temp1.c_str())), second(atof(temp3.c_str()));
			
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

	unsigned long u(blockEnd(buffer,start,"if","fi",path)), v(0);
	bool found(true);
	
	try
	{
		v = blockEnd(buffer,start,"if","else",path);
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
cgiProcessor::blockEnd(const dodoString &buffer, 
						unsigned long start, 
						const dodoString &st, 
						const dodoString &ts,
						const dodoString &path)
{
	unsigned long u, m(start), _st(1), b, p, stLen(st.size()), tsLen(ts.size());

	while (true)
	{
		u = buffer.find("<(",m);
		if (u == dodoString::npos)
			#ifndef NO_EX
			{
				char message[128];
				sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
				throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_BLOCKEND,ERR_LIBDODO,CGIPREPROCESSOR_WRONGBLOCK,CGIPREPROCESSOR_WRONGBLOCK_STR,__LINE__,__FILE__,message);
			}
			#else
				return start;
			#endif
			
		b = buffer.find(")>",u + 2);
		if (b == dodoString::npos)
			#ifndef NO_EX
			{
				char message[128];
				sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
				throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_BLOCKEND,ERR_LIBDODO,CGIPREPROCESSOR_WRONGBLOCK,CGIPREPROCESSOR_WRONGBLOCK_STR,__LINE__,__FILE__,message);
			}
			#else
				return start;
			#endif
		
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
cgiProcessor::_include(const dodoString &statement, 
						dodoString &tpl,
						unsigned long &start, 
						const dodoString &path)
{
	dodoString temp1 = getVar(statement,start,path);
	
	if (strcmp(temp1.c_str(),path.c_str()) != 0 && !recursive(temp1))
	{
		processed.push_back(path);
		tpl.append(process(temp1));
		processed.pop_back();
	}	
}						

//-------------------------------------------------------------------

void 
cgiProcessor::_print(const dodoString &statement, 
					dodoString &tpl, 
					unsigned long &start, 
					const dodoString &path)
{
	dodoStringArr temp = tools::explode(statement,",");
	if (temp.size() == 1)
		tpl.append(getVar(statement,start,path));	
	else
	{
		if (temp.size() > 1)
		{
			dodoStringArr::iterator i(temp.begin()), j(temp.end());
			for (;i!=j;++i)
				tpl.append(getVar(*i,start,path));
		}
	}
}

//-------------------------------------------------------------------

bool 
cgiProcessor::_break(const dodoString &statement, 
					unsigned long &start, 
					const dodoString &path)
{
	if (_loopDeepness > 0)
	{
		_breakDeepness = strtoul(getVar(statement,start,path).c_str(),NULL,10);
		
		if (_breakDeepness == 0)
			_breakDeepness = 1;	
		else
			if (_breakDeepness > _loopDeepness)
				_breakDeepness = _loopDeepness;
			
		return true;	
	}
	
	return false;
}

//-------------------------------------------------------------------

void 
cgiProcessor::_assign(const dodoString &statement, 
					unsigned long &start, 
					const dodoString &path)
{
	dodoStringArr temp = tools::explode(statement,"=",2);
		
	if (temp.size() == 0)	
		#ifndef NO_EX
		{
			char message[128];
			sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__ASSIGN,ERR_LIBDODO,CGIPREPROCESSOR_WRONGASSIGNSTATEMENT,CGIPREPROCESSOR_WRONGASSIGNSTATEMENT_STR,__LINE__,__FILE__,message);
		}
		#else
			return ;
		#endif
		
	dodoString varName = trim(temp[0]);	
	if (varName.size() == 0)	
		#ifndef NO_EX
		{
			char message[128];
			sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__ASSIGN,ERR_LIBDODO,CGIPREPROCESSOR_WRONGASSIGNSTATEMENT,CGIPREPROCESSOR_WRONGASSIGNSTATEMENT_STR,__LINE__,__FILE__,message);
		}
		#else
			return ;
		#endif
		
	if (varName[0] == '$')
		varName = varName.substr(1);
		
	if (strcmp(varName.c_str(),"dodo") == 0)
		#ifndef NO_EX
		{
			char message[128];
			sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__ASSIGN,ERR_LIBDODO,CGIPREPROCESSOR_DODOISRESERVEDVARNAME,CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR,__LINE__,__FILE__,message);
		}
		#else
			return ;
		#endif
	
	dodoAssocArr::iterator i = local.find(varName); 	
	if (i != local.end())
		if (localNamespace[namespaceDeepness].find(i->first) != localNamespace[namespaceDeepness].end())
			localNamespace[namespaceDeepness][i->first] = i->second;
	
	namespaceVars[namespaceDeepness].push_back(varName);
	local[varName] = getVar(temp[1],start,path);
}

//-------------------------------------------------------------------

void 
cgiProcessor::cleanNamespace()
{
	std::map<unsigned int, dodoStringArr>::iterator c = namespaceVars.find(namespaceDeepness);
	if (c != namespaceVars.end())
	{
		std::map<unsigned int, dodoAssocArr>::iterator v = localNamespace.find(namespaceDeepness);
		bool inLocal = (v != localNamespace.end())?true:false; 
		
		dodoStringArr::iterator x(c->second.begin()), z(c->second.end());
		for (;x!=z;++x)
			if (inLocal)
			{
				dodoAssocArr::iterator k = v->second.find(*x);
				if (k != v->second.end())
					local[k->first] = k->second;
				else
					local.erase(*x);
			}
			else
				local.erase(*x);
		
		namespaceVars.erase(namespaceDeepness);
		localNamespace.erase(namespaceDeepness);
	}
}

//-------------------------------------------------------------------

unsigned long 
cgiProcessor::_ns(const dodoString &buffer,
				unsigned long start,
				dodoString &tpl, 
				const dodoString &path)
{
	unsigned long u(blockEnd(buffer,start,"ns","sn",path));	
	
	tpl.append(_process(buffer.substr(start,u - start),path));
	
	return buffer.find(")>",u) + 2;
}

//-------------------------------------------------------------------

unsigned long 
cgiProcessor::_for(const dodoString &buffer,
				unsigned long start,
				const dodoString &statement,
				dodoString &tpl, 
				const dodoString &path)
{
	unsigned long u(blockEnd(buffer,start,"for","rof",path));
	
	unsigned long p = statement.find("$");
	unsigned long i(p), j(statement.size());
	
	for (;i<j;++i)
		if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
			break;
			
	dodoString varName = statement.substr(p,i - p).substr(1);

	dodoString keyVal;
	dodoAssocArr::iterator keyIter;
	bool key(false);	
	dodoString keyName;
	
	p = statement.find("=>",i + 1);
	if (p != dodoString::npos)
	{
		key = true;
		p = statement.find("$",p + 2);
		
		i = p;
		for (;i<j;++i)
			if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
				break;		
		
		keyName = varName;
		varName = statement.substr(p,i - p).substr(1);
	}

	p = statement.find("in",i + 1);
	if (p == dodoString::npos)
		#ifndef NO_EX
		{
			char message[128];
			sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
			throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR__FOR,ERR_LIBDODO,CGIPREPROCESSOR_WRONGFORSTATEMENT,CGIPREPROCESSOR_WRONGFORSTATEMENT_STR,__LINE__,__FILE__,message);
		}
		#else
			return u;
		#endif
	
	dodoString targetVar = trim(statement.substr(p + 2));
	dodoString forSpace = buffer.substr(start,u - start);
	
	u = buffer.find(")>",u) + 2;
					
	if (targetVar[0] == '$')
	{
		targetVar = targetVar.substr(1);
		
		dodoStringArr temp = tools::explode(targetVar,".");
		
		if (temp.size() == 1)
		{
			dodoAssocArr::iterator k = local.begin();
			dodoAssocArr::iterator l = local.end();		
			for (;k!=l;++k)
				if (strcmp(targetVar.c_str(),k->first.c_str()) == 0)
				{
					dodoAssocArr::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;
					
					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}
					
					unsigned long i(0),j(k->second.size());
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (;i<j;++i,++iterator)
					{
						dodo["iterator"] = tools::lToString(iterator);
						
						if (key)
							local[keyName] = tools::lToString(i);						
						local[varName] = dodoString(1,k->second[i]);							
						tpl.append(_process(forSpace,path));
						
						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}
					
					iterator =  iteratorPrev;
					dodo["iterator"] = tools::lToString(iteratorPrev);
					
					if (iter != local.end())
						local[varName] = iterVal;
					else
						local.erase(varName);
					
					if (key)
					{
						if (keyIter != local.end())
							local[keyName] = keyVal;
						else
							local.erase(keyName);						
					}
					
					return u;
				}

			std::map<dodoString, dodoAssocArr>::iterator g = localHash.begin();
			std::map<dodoString, dodoAssocArr>::iterator h = localHash.end();		
			for (;g!=h;++g)
				if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
				{
					dodoAssocArr::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;					

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}
					
					dodoAssocArr::iterator k = g->second.begin();
					dodoAssocArr::iterator l = g->second.end();		
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (;k!=l;++k,++iterator)
					{								
						dodo["iterator"] = tools::lToString(iterator);
									
						if (key)
							local[keyName] = k->first;
						local[varName] = k->second;
						tpl.append(_process(forSpace,path));
						
						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;		
					}
				
					iterator =  iteratorPrev;
					dodo["iterator"] = tools::lToString(iteratorPrev);
					
					if (iter != local.end())
						local[varName] = iterVal;
					else
						local.erase(varName);
					
					if (key)
					{
						if (keyIter != local.end())
							local[keyName] = keyVal;
						else
							local.erase(keyName);						
					}
					
					return u;
				}
		
			k = global.begin();
			l = global.end();		
			for (;k!=l;++k)
				if (strcmp(targetVar.c_str(),k->first.c_str()) == 0)
				{
					dodoAssocArr::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;	
					
					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}
																
					unsigned long i(0),j(k->second.size());
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (;i<j;++i,++iterator)
					{					
						dodo["iterator"] = tools::lToString(iterator);
						
						if (key)
							local[keyName] = tools::lToString(i);
						local[varName] = dodoString(1,k->second[i]);
						tpl.append(_process(forSpace,path));
						
						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}
						
					iterator =  iteratorPrev;
					dodo["iterator"] = tools::lToString(iteratorPrev);
					
					if (iter != local.end())
						local[varName] = iterVal;
					else
						local.erase(varName);
					
					if (key)
					{
						if (keyIter != local.end())
							local[keyName] = keyVal;
						else
							local.erase(keyName);						
					}
									
					return u;
				}

			g = globalHash.begin();
			h = globalHash.end();		
			for (;g!=h;++g)
				if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
				{
					dodoAssocArr::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;	
					
					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}
																
					dodoAssocArr::iterator k = g->second.begin();
					dodoAssocArr::iterator l = g->second.end();		
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (;k!=l;++k,++iterator)
					{								
						dodo["iterator"] = tools::lToString(iterator);
										
						if (key)
							local[keyName] = k->first;
						local[varName] = k->second;
						tpl.append(_process(forSpace,path));
						
						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}
					
					iterator =  iteratorPrev;	
					dodo["iterator"] = tools::lToString(iteratorPrev);
					
					if (iter != local.end())
						local[varName] = iterVal;
					else
						local.erase(varName);		
									
					if (key)
					{
						if (keyIter != local.end())
							local[keyName] = keyVal;
						else
							local.erase(keyName);						
					}
					
					return u;
				}
			
			std::map<dodoString, dodoStringArr>::iterator o = globalArray.begin();
			std::map<dodoString, dodoStringArr>::iterator p = globalArray.end();		
			for (;o!=p;++o)
				if (strcmp(temp[0].c_str(),o->first.c_str()) == 0)
				{
					dodoAssocArr::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;	

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}
											
					dodoStringArr::iterator k = o->second.begin();
					dodoStringArr::iterator l = o->second.end();	
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (unsigned long keyNIter(0);k!=l;++k,++keyNIter,++iterator)
					{					
						dodo["iterator"] = tools::lToString(iterator);
						
						if (key)
							local[keyName] = tools::lToString(keyNIter);
						local[varName] = *k;
						tpl.append(_process(forSpace,path));	
						
						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}	
						if (_continueFlag)
							_continueFlag = false;	
					}
					
					iterator =  iteratorPrev;	
					dodo["iterator"] = tools::lToString(iteratorPrev);	
					
					if (iter != local.end())
						local[varName] = iterVal;
					else
						local.erase(varName);		
									
					if (key)
					{
						if (keyIter != local.end())
							local[keyName] = keyVal;
						else
							local.erase(keyName);						
					}
					
					return u;
				}
				
			std::map<dodoString, dodoArray<dodoAssocArr> >::iterator d = globalArrayHash.begin();
			std::map<dodoString, dodoArray<dodoAssocArr> >::iterator f = globalArrayHash.end();		
			for (;d!=f;++d)
				if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
				{
					std::map<dodoString, dodoAssocArr>::iterator iter = localHash.find(varName);
					dodoAssocArr iterVal;
					if (iter != localHash.end())
						iterVal = iter->second;					

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}
										
					dodoArray<dodoAssocArr>::iterator k = d->second.begin();
					dodoArray<dodoAssocArr>::iterator l = d->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (unsigned long keyNIter(0);k!=l;++k,++keyNIter,++iterator)
					{		
						dodo["iterator"] = tools::lToString(iterator);
						
						if (key)
							local[keyName] = tools::lToString(keyNIter);
						localHash[varName] = *k;
						
						tpl.append(_process(forSpace,path));
						
						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}	
						if (_continueFlag)
							_continueFlag = false;
					}
					
					iterator =  iteratorPrev;	
					dodo["iterator"] = tools::lToString(iteratorPrev);
					
					if (iter != localHash.end())
						localHash[varName] = iterVal;
					else
						localHash.erase(varName);
									
					if (key)
					{
						if (keyIter != local.end())
							local[keyName] = keyVal;
						else
							local.erase(keyName);						
					}
									
					return u;
				}												
		}
		else
		{
			if (temp.size() == 2)
			{
				std::map<dodoString, dodoAssocArr>::iterator g = localHash.begin();
				std::map<dodoString, dodoAssocArr>::iterator h = localHash.end();		
				for (;g!=h;++g)
					if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
					{							
						dodoAssocArr::iterator k = g->second.begin();
						dodoAssocArr::iterator l = g->second.end();			
						for (;k!=l;++k)
							if (strcmp(temp[1].c_str(),k->first.c_str()) == 0)
							{
								dodoAssocArr::iterator iter = local.find(varName);
								dodoString iterVal;
								if (iter != local.end())
									iterVal = iter->second;	
								
								if (key)
								{
									keyIter = local.find(keyName);
									if (keyIter != local.end())
										keyVal = local[keyName];
								}
														
								unsigned long i(0),j(k->second.size());
								unsigned long iteratorPrev = iterator;
								iterator = 1;
								for (;i<j;++i,++iterator)
								{						
									dodo["iterator"] = tools::lToString(iterator);
						
									if (key)
										local[keyName] = tools::lToString(i);
									local[varName] = dodoString(1,k->second[i]);
									tpl.append(_process(forSpace,path));
									
									if (_breakDeepness > 0)
									{
										--_breakDeepness;
										break;
									}	
									if (_continueFlag)
										_continueFlag = false;	
								}
								
								iterator =  iteratorPrev;	
								dodo["iterator"] = tools::lToString(iteratorPrev);
								
								if (iter != local.end())
									local[varName] = iterVal;
								else
									local.erase(varName);
										
								if (key)
								{
									if (keyIter != local.end())
										local[keyName] = keyVal;
									else
										local.erase(keyName);						
								}
								
								return u;
							}
					}
							
				g = globalHash.begin();
				h = globalHash.end();		
				for (;g!=h;++g)
					if (strcmp(temp[0].c_str(),g->first.c_str()) == 0)
					{								
						dodoAssocArr::iterator k = g->second.begin();
						dodoAssocArr::iterator l = g->second.end();			
						for (;k!=l;++k)
							if (strcmp(temp[1].c_str(),k->first.c_str()) == 0)
							{
								dodoAssocArr::iterator iter = local.find(varName);
								dodoString iterVal;
								if (iter != local.end())
									iterVal = iter->second;	
								
								if (key)
								{
									keyIter = local.find(keyName);
									if (keyIter != local.end())
										keyVal = local[keyName];
								}
																									
								unsigned long i(0),j(k->second.size());
								unsigned long iteratorPrev = iterator;
								iterator = 1;
								for (;i<j;++i,++iterator)
								{			
									dodo["iterator"] = tools::lToString(iterator);
										
									if (key)
										local[keyName] = tools::lToString(i);
									local[varName] = dodoString(1,k->second[i]);
									tpl.append(_process(forSpace,path));
										
									if (_breakDeepness > 0)
									{
										--_breakDeepness;
										break;
									}
									if (_continueFlag)
										_continueFlag = false;
								}
								
								iterator =  iteratorPrev;	
								dodo["iterator"] = tools::lToString(iteratorPrev);
								
								if (iter != local.end())
									local[varName] = iterVal;
								else
									local.erase(varName);
										
								if (key)
								{
									if (keyIter != local.end())
										local[keyName] = keyVal;
									else
										local.erase(keyName);						
								}
												
								return u;
							}
					}
					
				std::map<dodoString, dodoStringArr>::iterator o = globalArray.begin();
				std::map<dodoString, dodoStringArr>::iterator p = globalArray.end();		
				for (;o!=p;++o)
					if (strcmp(temp[0].c_str(),o->first.c_str()) == 0)
					{
						unsigned long pos = atol(temp[1].c_str());
						if (pos >= 0 && pos <= o->second.size())
						{
							dodoAssocArr::iterator iter = local.find(varName);
							dodoString iterVal;
							if (iter != local.end())
								iterVal = iter->second;	
								
							if (key)
							{
								keyIter = local.find(keyName);
								if (keyIter != local.end())
									keyVal = local[keyName];
							}
																
							unsigned long i(0),j(o->second[pos].size());
							unsigned long iteratorPrev = iterator;
							iterator = 1;
							for (;i<j;++i,++iterator)
							{							
								dodo["iterator"] = tools::lToString(iterator);
						
								if (key)
									local[keyName] = tools::lToString(i);
								local[varName] = dodoString(1,o->second[pos][i]);
								tpl.append(_process(forSpace,path));
								
								if (_breakDeepness > 0)
								{
									--_breakDeepness;
									break;
								}
								if (_continueFlag)
									_continueFlag = false;	
							}
							
							iterator =  iteratorPrev;	
							dodo["iterator"] = tools::lToString(iteratorPrev);
							
							if (iter != local.end())
								local[varName] = iterVal;
							else
								local.erase(varName);

							if (key)
							{
								if (keyIter != local.end())
									local[keyName] = keyVal;
								else
									local.erase(keyName);						
							}
											
							return u;
						}
					}							
	
				std::map<dodoString, dodoArray<dodoAssocArr> >::iterator d = globalArrayHash.begin();
				std::map<dodoString, dodoArray<dodoAssocArr> >::iterator f = globalArrayHash.end();
				for (;d!=f;++d)
					if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
					{
						unsigned long pos = atol(temp[1].c_str());
						if (pos >= 0 && pos <= d->second.size())
						{
							dodoAssocArr::iterator iter = local.find(varName);
							dodoString iterVal;
							if (iter != local.end())
								iterVal = iter->second;	
								
							if (key)
							{
								keyIter = local.find(keyName);
								if (keyIter != local.end())
									keyVal = local[keyName];
							}
																							
							dodoAssocArr::iterator k = d->second[pos].begin();					
							dodoAssocArr::iterator l = d->second[pos].end();
							unsigned long iteratorPrev = iterator;
							iterator = 1;
							for (;k!=l;++k,++iterator)
							{	
								dodo["iterator"] = tools::lToString(iterator);
								
								if (key)
									local[keyName] = k->first;
								local[varName] = k->second;
								tpl.append(_process(forSpace,path));	
								
								if (_breakDeepness > 0)
								{
									--_breakDeepness;
									break;
								}	
								if (_continueFlag)
									_continueFlag = false;
							}
							
							iterator =  iteratorPrev;	
							dodo["iterator"] = tools::lToString(iteratorPrev);	
							
							if (iter != local.end())
								local[varName] = iterVal;
							else
								local.erase(varName);

							if (key)
							{
								if (keyIter != local.end())
									local[keyName] = keyVal;
								else
									local.erase(keyName);						
							}
										
							return u;
						}
					}
			}
			else	
			{
				if (temp.size() == 3)
				{
					std::map<dodoString, dodoArray<dodoAssocArr> >::iterator d = globalArrayHash.begin();
					std::map<dodoString, dodoArray<dodoAssocArr> >::iterator f = globalArrayHash.end();
					for (;d!=f;++d)
						if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
						{
							unsigned long pos = atol(temp[1].c_str());
							if (pos >= 0 && pos <= d->second.size())
							{
								dodoAssocArr::iterator k = d->second[pos].begin();					
								dodoAssocArr::iterator l = d->second[pos].end();
								for (;k!=l;++k)
									if (strcmp(temp[2].c_str(),k->first.c_str()) == 0)
									{
										dodoAssocArr::iterator iter = local.find(varName);
										dodoString iterVal;
										if (iter != local.end())
											iterVal = iter->second;	
								
										if (key)
										{
											keyIter = local.find(keyName);
											if (keyIter != local.end())
												keyVal = local[keyName];
										}		
																
										unsigned long i(0),j(k->second.size());
										unsigned long iteratorPrev = iterator;
										iterator = 1;
										for (;i<j;++i,++iterator)
										{
											dodo["iterator"] = tools::lToString(iterator);
						
											if (key)
												local[keyName] = tools::lToString(i);
											local[varName] = dodoString(1,k->second[i]);
											tpl.append(_process(forSpace,path));
											
											if (_breakDeepness > 0)
											{
												--_breakDeepness;
												break;
											}	
											if (_continueFlag)
												_continueFlag = false;	
										}
										
										iterator =  iteratorPrev;	
										dodo["iterator"] = tools::lToString(iteratorPrev);
										
										if (iter != local.end())
											local[varName] = iterVal;
										else
											local.erase(varName);

										if (key)
										{
											if (keyIter != local.end())
												local[keyName] = keyVal;
											else
												local.erase(keyName);						
										}
											
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

		dodoAssocArr::iterator iter = local.find(varName);
		dodoString iterVal;
		if (iter != local.end())
			iterVal = iter->second;	
			
		if (key)
		{
			keyIter = local.find(keyName);
			if (keyIter != local.end())
				keyVal = local[keyName];
		}					
			
		unsigned long i(0), j(targetVar.size());
		unsigned long iteratorPrev = iterator;
		iterator = 1;
		for (;i<j;++i,++iterator)
		{		
			dodo["iterator"] = tools::lToString(iterator);
						
			if (key)
				local[keyName] = tools::lToString(i);
			local[varName] = dodoString(1,targetVar[i]);
			tpl.append(_process(forSpace,path));	
			
			if (_breakDeepness > 0)
			{
				--_breakDeepness;
				break;
			}		
			if (_continueFlag)
				_continueFlag = false;	
		}
		
		iterator =  iteratorPrev;	
		dodo["iterator"] = tools::lToString(iteratorPrev);
		
		if (iter != local.end())
			local[varName] = iterVal;
		else
			local.erase(varName);

		if (key)
		{
			if (keyIter != local.end())
				local[keyName] = keyVal;
			else
				local.erase(keyName);						
		}
					
		return u;
	}
					
	return u;	
}					

//-------------------------------------------------------------------

dodoString 
cgiProcessor::getVar(const dodoString &a_varName, 
					unsigned long &start, 
					const dodoString &path)
{
	dodoString varName = trim(a_varName), tempVar;
	unsigned long u, b, m(0), ob, cb, i, c;
	
	while (true)
	{
		ob = 1;
		cb = 0;
		
		u = varName.find("{",m);
		if (u == dodoString::npos)
			break;
		
		c = u;
		while (true)	
		{
			b = varName.find("}",c + 1);
			if (b == dodoString::npos)
				#ifndef NO_EX
				{
					char message[128];
					sprintf(message," Line: %li File: %s",getLineNumber(newLinePositions.back(),start),path.c_str());
					throw baseEx(ERRMODULE_CGIPROCESSOR,CGIPROCESSOR_GETVAR,ERR_LIBDODO,CGIPREPROCESSOR_WRONGVARSTATEMENT,CGIPREPROCESSOR_WRONGVARSTATEMENT_STR,__LINE__,__FILE__,message);
				}
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
			tempVar = getVar(tempVar,start,path);
		
		varName.replace(u,b + 1 - u,tempVar,0,tempVar.size());
							
		m = b - 1;	
	}	
	
	if (varName[0] != '$')
		return varName;
	
	varName.erase(0,1);
	
	dodoStringArr temp = tools::explode(varName,".");
	
	if (temp.size() == 1)
	{	
		if (strcmp(varName.c_str(),"dodo") == 0)			
			return "cgi framework libdodo";
		
		dodoAssocArr::iterator k(local.begin()), l(local.end());		
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
		if (strcmp(temp[0].c_str(),"dodo") == 0)
		{
			dodoAssocArr::iterator k(local.begin()), l(local.end());	
			for (;k!=l;++k)
				if (strcmp(temp[1].c_str(),k->first.c_str()) == 0)
					if (temp.size() == 3)
					{
						unsigned long pos = atol(temp[2].c_str());
						if (pos >= 0 && pos <= k->second.size())
							return dodoString(1,k->second[pos]);
						else
							return __string__;
					}
					else
						return k->second;
					
			return __string__;
		}
		
		dodoAssocArr::iterator k(local.begin()), l(local.end());		
		for (;k!=l;++k)
			if (strcmp(temp[0].c_str(),k->first.c_str()) == 0)
			{
				unsigned long pos = atol(temp[1].c_str());
				if (pos >= 0 && pos <= k->second.size())
					return dodoString(1,k->second[pos]);
				else
					return __string__;
			}
			
		std::map<dodoString, dodoAssocArr>::iterator g(localHash.begin()), h(localHash.end());		
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
							unsigned long pos = atol(temp[2].c_str());
							if (pos >= 0 && pos <= k->second.size())
								return dodoString(1,k->second[pos]);
							else
								return __string__;
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
				unsigned long pos = atol(temp[1].c_str());
				if (pos >= 0 && pos <= k->second.size())
					return dodoString(1,k->second[pos]);
				else
					return __string__;
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
							unsigned long pos = atol(temp[2].c_str());
							if (pos >= 0 && pos <= k->second.size())
								return dodoString(1,k->second[pos]);
							else
								return __string__;
						}
						else
							return k->second;
					}
			}
			
		std::map<dodoString, dodoStringArr>::iterator o(globalArray.begin()), p(globalArray.end());		
		for (;o!=p;++o)
			if (strcmp(temp[0].c_str(),o->first.c_str()) == 0)
			{
				unsigned long pos = atol(temp[1].c_str());
				if (pos >= 0 && pos <= o->second.size())
				{
					if (temp.size() == 3)
					{
						unsigned long pos1 = atol(temp[2].c_str());
						if (pos >= 0 && pos1 <= o->second[pos].size())
							return dodoString(1,o->second[pos][pos1]);
						else
							return __string__;
					}
					else
						return o->second[pos];
				}
			}							
		
		if (temp.size() >= 3)	
		{
			std::map<dodoString, dodoArray<dodoAssocArr> >::iterator d(globalArrayHash.begin()), f(globalArrayHash.end());
			for (;d!=f;++d)
				if (strcmp(temp[0].c_str(),d->first.c_str()) == 0)
				{
					unsigned long pos = atol(temp[1].c_str());
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
										return dodoString(1,k->second[pos]);
									else
										return __string__;
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

dodoString 
cgiProcessor::trim(const dodoString &statement)
{
	dodoString temp = tools::trim(statement," \t\n",3);
	
	unsigned long i(temp.size() - 1);
	
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

void 
cgiProcessor::display(const dodoString &path)
{
		#ifdef FCGI_EXT
			if (cgiFastSet)
			{
				cf->print(this->process(path));
				cf->flush();
			}
			else
		#endif
			{		
				fstd->writeStreamString(this->process(path));
				fstd->flush();
			}
}

//-------------------------------------------------------------------
