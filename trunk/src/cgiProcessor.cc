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

#include <libdodo/cgiProcessor.h>

using namespace dodo;

cgiProcessor::cgiProcessor(cgi &a_cgi) : continueFlag(false),
										 breakDeepness(0),
										 loopDeepness(0),
										 iterator(1),
										 namespaceDeepness(1),
										 CGI(a_cgi)
{
	dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_VERSION]] = PACKAGE_STRING;
	dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = "1";
}

//-------------------------------------------------------------------

cgiProcessor::~cgiProcessor()
{
}

//-------------------------------------------------------------------

dodoString
cgiProcessor::processString(const dodoString &tpl)
{
	dodoString tmp = _processString(preProcessString(tpl), "memory");

	newLinePositions.pop_back();

	return tmp;
}

//-------------------------------------------------------------------

dodoString
cgiProcessor::process(const dodoString &path)
{
	dodoString tmp = _processString(preProcess(path), path);

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
cgiProcessor::_processString(const dodoString &buffer,
							 const dodoString &path)
{
	unsigned long i(0), j(0), begin(0), k(0);

	dodoString tpl;
	dodoString temp;

	while (true)
	{
		begin = j;

		i = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST], begin);
		if (i == dodoString::npos)
		{
			tpl.append(buffer.substr(begin));
			break;
		}
		else
			tpl.append(buffer.substr(begin, i - begin));

		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NP], i);

			++i;
			tpl.append(buffer.substr(i, j - i));
			j += 3;

			continue;
		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], i);

			j += 3;

			continue;
		}

		j = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], i);

		temp = buffer.substr(i, j - i);

		j += 2;

		k = temp.find(statements[CGIPREPROCESSOR_CGIPREPROCESSOR_PROCESSORSTATEMENT_INCLUDE]);
		if (k != dodoString::npos)
		{
			j = _include(j, temp.substr(k + 8), tpl, path);
		}
		else
		{
			k = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF]);
			if (k != dodoString::npos)
			{
				++namespaceDeepness;

				j = _if(buffer, j, temp.substr(k + 2), tpl, path);

				cleanNamespace();

				--namespaceDeepness;
			}
			else
			{
				k = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_PRINT]);
				if (k != dodoString::npos)
				{
					j = _print(j, temp.substr(k + 5), tpl, path);
				}
				else
				{
					k = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_FOR]);
					if (k != dodoString::npos)
					{
						++loopDeepness;
						++namespaceDeepness;

						j = _for(buffer, j, temp.substr(k + 3), tpl, path);

						cleanNamespace();

						--namespaceDeepness;
						--loopDeepness;
					}
					else
					{
						k = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_BREAK]);
						if (k != dodoString::npos)
						{
							if (_break(j, temp.substr(k + 5), path))
								break;
						}
						else
						{
							k = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CONT]);
							if (k != dodoString::npos)
							{
								if (loopDeepness > 0)
								{
									continueFlag = true;
									break;
								}
							}
							else
							{
								k = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ASSIGN]);
								if (k != dodoString::npos)
								{
									j = _assign(j, temp.substr(k + 6), path);
								}
								else
								{
									k = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_NS]);
									if (k != dodoString::npos)
									{
										++namespaceDeepness;

										j = _ns(buffer, j, tpl, path);

										cleanNamespace();

										--namespaceDeepness;
									}
									else
										tpl.append(buffer.substr(i - 2, j - i + 2));
								}
							}
						}
					}
				}
			}
		}

		if (breakDeepness > 0)
			break;

		if (continueFlag)
			break;
	}

	return tpl;
}

//-------------------------------------------------------------------

bool
cgiProcessor::recursive(const dodoString &path)
{
	dodoList<dodoString>::iterator i(processed.begin()), j(processed.end());
	for (; i != j; ++i)
		if (toolsString::equal(*i, path))
			return true;

	return false;
}

//-------------------------------------------------------------------

void
cgiProcessor::assign(dodoString varName,
					 const dodoArray<dodoStringMap> &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_ASSIGN, ERR_LIBDODO, CGIPROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalArray.erase(varName);
	globalHash.erase(varName);

	globalArrayHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
cgiProcessor::assign(dodoString varName,
					 const dodoStringMap &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_ASSIGN, ERR_LIBDODO, CGIPROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalArray.erase(varName);
	globalArrayHash.erase(varName);

	globalHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
cgiProcessor::assign(dodoString varName,
					 const dodoStringArray &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_ASSIGN, ERR_LIBDODO, CGIPROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

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

	if (toolsString::equal(varName, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_ASSIGN, ERR_LIBDODO, CGIPROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

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

	dodoArray<dodoString> temp2 = tools::explode(statement, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_EQ]);
	if (temp2.size() != 2)
	{
		temp2 = tools::explode(statement, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_NE]);
		if (temp2.size() != 2)
		{
			temp2 = tools::explode(statement, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_LE]);
			if (temp2.size() != 2)
			{
				temp2 = tools::explode(statement, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_GE]);
				if (temp2.size() != 2)
				{
					temp2 = tools::explode(statement, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_LT]);
					if (temp2.size() != 2)
					{
						temp2 = tools::explode(statement, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_GT]);
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
			throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX__IF, ERR_LIBDODO, CGIPROCESSOREX_WRONGIFSTATEMENT, CGIPROCESSOREX_WRONGIFSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

		dodoString temp1 = toolsString::trim(temp2[0], " \t\n", 3);

		if (temp1[0] == '!')
		{
			invert = true;
			temp1 = temp1.substr(1);
		}

		temp1 = getVar(temp1, start, path);

		if (!toolsString::equal(temp1, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_FALSE]) && temp1.size() != 0)
			accept = !invert;
		else
			accept = invert;
	}
	else
	{
		dodoString temp1 = getVar(temp2[0], start, path);

		dodoString temp3 = getVar(temp2[1], start, path);

		if (_float)
		{
			double first(toolsString::stringToD(temp1)), second(toolsString::stringToD(temp3));

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
			if (toolsString::equal(temp1, temp3))
				accept = !invert;
		}
	}

	unsigned long u(blockEnd(buffer, start, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF], statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_IF], path)), v(0);
	bool found(true);

	try
	{
		v = blockEnd(buffer, start, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF], statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ELSE], path);
	}
	catch (...)
	{
		found = false;
	}

	if (accept)
	{
		if (!found)
			v = u;

		tpl.append(_processString(buffer.substr(start, v - start), path));
	}
	else
	{
		if (found)
		{
			v = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], v) + 2;
			tpl.append(_processString(buffer.substr(v, u - v), path));
		}
	}

	return buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u) + 2;
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
		u = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST], m);
		if (u == dodoString::npos)
			throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_BLOCKEND, ERR_LIBDODO, CGIPROCESSOREX_WRONGBLOCK, CGIPROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

		b = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u + 2);
		if (b == dodoString::npos)
			throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_BLOCKEND, ERR_LIBDODO, CGIPROCESSOREX_WRONGBLOCK, CGIPROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

		for (p = u; p < b; ++p)
			if (buffer[p] != ' ' && buffer[p] != '\t' && buffer[p] != '\n')
			{
				if (toolsString::equal(buffer.substr(p, tsLen), ts))
					--_st;
				else
				{
					if (toolsString::equal(buffer.substr(p, stLen), st))
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

unsigned long
cgiProcessor::_include(unsigned long start,
					const dodoString &statement,
					   dodoString &tpl,
					   const dodoString &path)
{
	dodoString temp1 = getVar(statement, start, path);

	if (!toolsString::equal(temp1, path) && !recursive(temp1) && toolsFilesystem::exists(tplBasePath + FILE_DELIM + temp1))
	{
		processed.push_back(path);
		tpl.append(process(temp1));
		processed.pop_back();
	}
	
	return start;
}

//-------------------------------------------------------------------

unsigned long
cgiProcessor::_print(unsigned long start,
					const dodoString &statement,
					 dodoString &tpl,
					 const dodoString &path)
{
	dodoStringArray temp = tools::explode(statement, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_COMA]);
	if (temp.size() == 1)
		tpl.append(getVar(statement, start, path));
	else
	{
		if (temp.size() > 1)
		{
			dodoStringArray::iterator i(temp.begin()), j(temp.end());
			for (; i != j; ++i)
				tpl.append(getVar(*i, start, path));
		}
	}
	
	return start;
}

//-------------------------------------------------------------------

bool
cgiProcessor::_break(unsigned long start,
		 			const dodoString &statement,
					 const dodoString &path)
{
	if (loopDeepness > 0)
	{
		breakDeepness = toolsString::stringToUL(getVar(statement, start, path));

		if (breakDeepness == 0)
			breakDeepness = 1;
		else
			if (breakDeepness > loopDeepness)
				breakDeepness = loopDeepness;

		return true;
	}

	return false;
}

//-------------------------------------------------------------------

unsigned long
cgiProcessor::_assign(unsigned long start,
						const dodoString &statement,
					  const dodoString &path)
{
	dodoStringArray temp = tools::explode(statement, statements[CGIPREPROCESSOR_CGIPREPROCESSOR_PROCESSORSTATEMENT_ASSIGN_OP], 2);

	if (temp.size() == 0)
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_ASSIGN, ERR_LIBDODO, CGIPROCESSOREX_WRONGASSIGNSTATEMENT, CGIPROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodoString varName = trim(temp[0]);
	if (varName.size() == 0)
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_ASSIGN, ERR_LIBDODO, CGIPROCESSOREX_WRONGASSIGNSTATEMENT, CGIPROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_ASSIGN, ERR_LIBDODO, CGIPROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodoStringMap::iterator i = local.find(varName);
	if (i != local.end())
		if (localNamespace[namespaceDeepness].find(i->first) != localNamespace[namespaceDeepness].end())
			localNamespace[namespaceDeepness][i->first] = i->second;

	namespaceVars[namespaceDeepness].push_back(varName);
	local[varName] = getVar(temp[1], start, path);
	
	return start;
}

//-------------------------------------------------------------------

void
cgiProcessor::cleanNamespace()
{
	dodoMap<unsigned int, dodoStringArray>::iterator c = namespaceVars.find(namespaceDeepness);
	if (c != namespaceVars.end())
	{
		dodoMap<unsigned int, dodoStringMap>::iterator v = localNamespace.find(namespaceDeepness);
		bool inLocal = v != localNamespace.end() ? true : false;

		dodoStringArray::iterator x(c->second.begin()), z(c->second.end());
		for (; x != z; ++x)
			if (inLocal)
			{
				dodoStringMap::iterator k = v->second.find(*x);
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
	unsigned long u(blockEnd(buffer, start, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_NS], statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NS], path));

	tpl.append(_processString(buffer.substr(start, u - start), path));

	return buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
cgiProcessor::_for(const dodoString &buffer,
				   unsigned long start,
				   const dodoString &statement,
				   dodoString &tpl,
				   const dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_FOR], statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_FOR], path));

	unsigned long p = statement.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DOLLAR]);
	unsigned long i(p), j(statement.size());

	for (; i < j; ++i)
		if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
			break;

	dodoString varName = statement.substr(p, i - p).substr(1);

	dodoString keyVal;
	dodoStringMap::iterator keyIter;
	bool key(false);
	dodoString keyName;

	p = statement.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_KEY_VALUE], i + 1);
	if (p != dodoString::npos)
	{
		key = true;
		p = statement.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DOLLAR], p + 2);

		i = p;
		for (; i < j; ++i)
			if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
				break;

		keyName = varName;
		varName = statement.substr(p, i - p).substr(1);
	}

	p = statement.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_IN], i + 1);
	if (p == dodoString::npos)
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX__FOR, ERR_LIBDODO, CGIPROCESSOREX_WRONGFORSTATEMENT, CGIPROCESSOREX_WRONGFORSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodoString targetVar = trim(statement.substr(p + 2));
	dodoString forSpace = buffer.substr(start, u - start);

	u = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u) + 2;

	if (targetVar[0] == '$')
	{
		targetVar = targetVar.substr(1);

		dodoStringArray temp = tools::explode(targetVar, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DOT]);

		if (temp.size() == 1)
		{
			dodoStringMap::iterator k = local.begin();
			dodoStringMap::iterator l = local.end();
			for (; k != l; ++k)
				if (toolsString::equal(targetVar, k->first))
				{
					dodoStringMap::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					unsigned long i(0), j(k->second.size());
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (; i < j; ++i, ++iterator)
					{
						dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = toolsString::lToString(i);
						local[varName] = dodoString(1, k->second[i]);
						tpl.append(_processString(forSpace, path));

						if (breakDeepness > 0)
						{
							--breakDeepness;
							break;
						}
						if (continueFlag)
							continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

			dodoMap<dodoString, dodoStringMap>::iterator g = localHash.begin();
			dodoMap<dodoString, dodoStringMap>::iterator h = localHash.end();
			for (; g != h; ++g)
				if (toolsString::equal(temp[0], g->first))
				{
					dodoStringMap::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodoStringMap::iterator k = g->second.begin();
					dodoStringMap::iterator l = g->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (; k != l; ++k, ++iterator)
					{
						dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = k->first;
						local[varName] = k->second;
						tpl.append(_processString(forSpace, path));

						if (breakDeepness > 0)
						{
							--breakDeepness;
							break;
						}
						if (continueFlag)
							continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
			for (; k != l; ++k)
				if (toolsString::equal(targetVar, k->first))
				{
					dodoStringMap::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					unsigned long i(0), j(k->second.size());
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (; i < j; ++i, ++iterator)
					{
						dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = toolsString::lToString(i);
						local[varName] = dodoString(1, k->second[i]);
						tpl.append(_processString(forSpace, path));

						if (breakDeepness > 0)
						{
							--breakDeepness;
							break;
						}
						if (continueFlag)
							continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
			for (; g != h; ++g)
				if (toolsString::equal(temp[0], g->first))
				{
					dodoStringMap::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodoStringMap::iterator k = g->second.begin();
					dodoStringMap::iterator l = g->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (; k != l; ++k, ++iterator)
					{
						dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = k->first;
						local[varName] = k->second;
						tpl.append(_processString(forSpace, path));

						if (breakDeepness > 0)
						{
							--breakDeepness;
							break;
						}
						if (continueFlag)
							continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

			dodoMap<dodoString, dodoStringArray>::iterator o = globalArray.begin();
			dodoMap<dodoString, dodoStringArray>::iterator p = globalArray.end();
			for (; o != p; ++o)
				if (toolsString::equal(temp[0], o->first))
				{
					dodoStringMap::iterator iter = local.find(varName);
					dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodoStringArray::iterator k = o->second.begin();
					dodoStringArray::iterator l = o->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (unsigned long keyNIter(0); k != l; ++k, ++keyNIter, ++iterator)
					{
						dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = toolsString::lToString(keyNIter);
						local[varName] = *k;
						tpl.append(_processString(forSpace, path));

						if (breakDeepness > 0)
						{
							--breakDeepness;
							break;
						}
						if (continueFlag)
							continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

			dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.begin();
			dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator f = globalArrayHash.end();
			for (; d != f; ++d)
				if (toolsString::equal(temp[0], d->first))
				{
					dodoMap<dodoString, dodoStringMap>::iterator iter = localHash.find(varName);
					dodoStringMap iterVal;
					if (iter != localHash.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodoArray<dodoStringMap>::iterator k = d->second.begin();
					dodoArray<dodoStringMap>::iterator l = d->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (unsigned long keyNIter(0); k != l; ++k, ++keyNIter, ++iterator)
					{
						dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = toolsString::lToString(keyNIter);
						localHash[varName] = *k;

						tpl.append(_processString(forSpace, path));

						if (breakDeepness > 0)
						{
							--breakDeepness;
							break;
						}
						if (continueFlag)
							continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
				dodoMap<dodoString, dodoStringMap>::iterator g = localHash.begin();
				dodoMap<dodoString, dodoStringMap>::iterator h = localHash.end();
				for (; g != h; ++g)
					if (toolsString::equal(temp[0], g->first))
					{
						dodoStringMap::iterator k = g->second.begin();
						dodoStringMap::iterator l = g->second.end();
						for (; k != l; ++k)
							if (toolsString::equal(temp[1], k->first))
							{
								dodoStringMap::iterator iter = local.find(varName);
								dodoString iterVal;
								if (iter != local.end())
									iterVal = iter->second;

								if (key)
								{
									keyIter = local.find(keyName);
									if (keyIter != local.end())
										keyVal = local[keyName];
								}

								unsigned long i(0), j(k->second.size());
								unsigned long iteratorPrev = iterator;
								iterator = 1;
								for (; i < j; ++i, ++iterator)
								{
									dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

									if (key)
										local[keyName] = toolsString::lToString(i);
									local[varName] = dodoString(1, k->second[i]);
									tpl.append(_processString(forSpace, path));

									if (breakDeepness > 0)
									{
										--breakDeepness;
										break;
									}
									if (continueFlag)
										continueFlag = false;
								}

								iterator =  iteratorPrev;
								dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
				for (; g != h; ++g)
					if (toolsString::equal(temp[0], g->first))
					{
						dodoStringMap::iterator k = g->second.begin();
						dodoStringMap::iterator l = g->second.end();
						for (; k != l; ++k)
							if (toolsString::equal(temp[1], k->first))
							{
								dodoStringMap::iterator iter = local.find(varName);
								dodoString iterVal;
								if (iter != local.end())
									iterVal = iter->second;

								if (key)
								{
									keyIter = local.find(keyName);
									if (keyIter != local.end())
										keyVal = local[keyName];
								}

								unsigned long i(0), j(k->second.size());
								unsigned long iteratorPrev = iterator;
								iterator = 1;
								for (; i < j; ++i, ++iterator)
								{
									dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

									if (key)
										local[keyName] = toolsString::lToString(i);
									local[varName] = dodoString(1, k->second[i]);
									tpl.append(_processString(forSpace, path));

									if (breakDeepness > 0)
									{
										--breakDeepness;
										break;
									}
									if (continueFlag)
										continueFlag = false;
								}

								iterator =  iteratorPrev;
								dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

				dodoMap<dodoString, dodoStringArray>::iterator o = globalArray.begin();
				dodoMap<dodoString, dodoStringArray>::iterator p = globalArray.end();
				for (; o != p; ++o)
					if (toolsString::equal(temp[0], o->first))
					{
						unsigned long pos = toolsString::stringToUL(temp[1]);
						if (pos >= 0 && pos <= o->second.size())
						{
							dodoStringMap::iterator iter = local.find(varName);
							dodoString iterVal;
							if (iter != local.end())
								iterVal = iter->second;

							if (key)
							{
								keyIter = local.find(keyName);
								if (keyIter != local.end())
									keyVal = local[keyName];
							}

							unsigned long i(0), j(o->second[pos].size());
							unsigned long iteratorPrev = iterator;
							iterator = 1;
							for (; i < j; ++i, ++iterator)
							{
								dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

								if (key)
									local[keyName] = toolsString::lToString(i);
								local[varName] = dodoString(1, o->second[pos][i]);
								tpl.append(_processString(forSpace, path));

								if (breakDeepness > 0)
								{
									--breakDeepness;
									break;
								}
								if (continueFlag)
									continueFlag = false;
							}

							iterator =  iteratorPrev;
							dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

				dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.begin();
				dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator f = globalArrayHash.end();
				for (; d != f; ++d)
					if (toolsString::equal(temp[0], d->first))
					{
						unsigned long pos = toolsString::stringToUL(temp[1]);
						if (pos >= 0 && pos <= d->second.size())
						{
							dodoStringMap::iterator iter = local.find(varName);
							dodoString iterVal;
							if (iter != local.end())
								iterVal = iter->second;

							if (key)
							{
								keyIter = local.find(keyName);
								if (keyIter != local.end())
									keyVal = local[keyName];
							}

							dodoStringMap::iterator k = d->second[pos].begin();
							dodoStringMap::iterator l = d->second[pos].end();
							unsigned long iteratorPrev = iterator;
							iterator = 1;
							for (; k != l; ++k, ++iterator)
							{
								dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

								if (key)
									local[keyName] = k->first;
								local[varName] = k->second;
								tpl.append(_processString(forSpace, path));

								if (breakDeepness > 0)
								{
									--breakDeepness;
									break;
								}
								if (continueFlag)
									continueFlag = false;
							}

							iterator =  iteratorPrev;
							dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
					dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.begin();
					dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator f = globalArrayHash.end();
					for (; d != f; ++d)
						if (toolsString::equal(temp[0], d->first))
						{
							unsigned long pos = toolsString::stringToUL(temp[1]);
							if (pos >= 0 && pos <= d->second.size())
							{
								dodoStringMap::iterator k = d->second[pos].begin();
								dodoStringMap::iterator l = d->second[pos].end();
								for (; k != l; ++k)
									if (toolsString::equal(temp[2], k->first))
									{
										dodoStringMap::iterator iter = local.find(varName);
										dodoString iterVal;
										if (iter != local.end())
											iterVal = iter->second;

										if (key)
										{
											keyIter = local.find(keyName);
											if (keyIter != local.end())
												keyVal = local[keyName];
										}

										unsigned long i(0), j(k->second.size());
										unsigned long iteratorPrev = iterator;
										iterator = 1;
										for (; i < j; ++i, ++iterator)
										{
											dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

											if (key)
												local[keyName] = toolsString::lToString(i);
											local[varName] = dodoString(1, k->second[i]);
											tpl.append(_processString(forSpace, path));

											if (breakDeepness > 0)
											{
												--breakDeepness;
												break;
											}
											if (continueFlag)
												continueFlag = false;
										}

										iterator =  iteratorPrev;
										dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

		dodoStringMap::iterator iter = local.find(varName);
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
		for (; i < j; ++i, ++iterator)
		{
			dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

			if (key)
				local[keyName] = toolsString::lToString(i);
			local[varName] = dodoString(1, targetVar[i]);
			tpl.append(_processString(forSpace, path));

			if (breakDeepness > 0)
			{
				--breakDeepness;
				break;
			}
			if (continueFlag)
				continueFlag = false;
		}

		iterator =  iteratorPrev;
		dodo[statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
					 unsigned long start,
					 const dodoString &path)
{
	dodoString varName = trim(a_varName), tempVar;
	unsigned long u, b, m(0), ob, cb, i, c;

	while (true)
	{
		ob = 1;
		cb = 0;

		u = varName.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_VARPART], m);
		if (u == dodoString::npos)
			break;

		c = u;
		while (true)
		{
			b = varName.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_VARPART], c + 1);
			if (b == dodoString::npos)
				throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOREX_GETVAR, ERR_LIBDODO, CGIPROCESSOREX_WRONGVARSTATEMENT, CGIPROCESSOREX_WRONGVARSTATEMENT_STR, __LINE__, __FILE__,toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

			++cb;

			for (i = c + 1; i < b; ++i)
				if (varName[i] == '{')
					++ob;

			c = b;

			if (cb == ob)
				break;
		}

		tempVar = trim(varName.substr(u + 1, b - 1 - u));
		if (tempVar[0] == '$')
			tempVar = getVar(tempVar, start, path);

		varName.replace(u, b + 1 - u, tempVar, 0, tempVar.size());

		m = b - 1;
	}

	if (varName[0] != '$')
		return varName;

	varName.erase(0, 1);

	dodoStringArray temp = tools::explode(varName, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DOT]);

	if (temp.size() == 1)
	{
		if (toolsString::equal(varName, statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO]))
			return "cgi framework libdodo";

		dodoStringMap::iterator k(local.begin()), l(local.end());
		for (; k != l; ++k)
			if (toolsString::equal(varName, k->first))
				return k->second;

		k = global.begin();
		l = global.end();
		for (; k != l; ++k)
			if (toolsString::equal(varName, k->first))
				return k->second;
	}
	else
	{
		if (toolsString::equal(temp[0], statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		{
			dodoStringMap::iterator k(local.begin()), l(local.end());
			for (; k != l; ++k)
				if (toolsString::equal(temp[1], k->first))
					if (temp.size() == 3)
					{
						unsigned long pos = toolsString::stringToUL(temp[2]);
						if (pos >= 0 && pos <= k->second.size())
							return dodoString(1, k->second[pos]);
						else
							return __dodostring__;
					}
					else
						return k->second;

			return __dodostring__;
		}

		dodoStringMap::iterator k(local.begin()), l(local.end());
		for (; k != l; ++k)
			if (toolsString::equal(temp[0], k->first))
			{
				unsigned long pos = toolsString::stringToUL(temp[1]);
				if (pos >= 0 && pos <= k->second.size())
					return dodoString(1, k->second[pos]);
				else
					return __dodostring__;
			}

		dodoMap<dodoString, dodoStringMap>::iterator g(localHash.begin()), h(localHash.end());
		for (; g != h; ++g)
			if (toolsString::equal(temp[0], g->first))
			{
				k = g->second.begin();
				l = g->second.end();
				for (; k != l; ++k)
					if (toolsString::equal(temp[1], k->first))
					{
						if (temp.size() == 3)
						{
							unsigned long pos = toolsString::stringToUL(temp[2]);
							if (pos >= 0 && pos <= k->second.size())
								return dodoString(1, k->second[pos]);
							else
								return __dodostring__;
						}
						else
							return k->second;
					}
			}

		k = global.begin();
		l = global.end();
		for (; k != l; ++k)
			if (toolsString::equal(temp[0], k->first))
			{
				unsigned long pos = toolsString::stringToUL(temp[1]);
				if (pos >= 0 && pos <= k->second.size())
					return dodoString(1, k->second[pos]);
				else
					return __dodostring__;
			}

		g = globalHash.begin();
		h = globalHash.end();
		for (; g != h; ++g)
			if (toolsString::equal(temp[0], g->first))
			{
				k = g->second.begin();
				l = g->second.end();
				for (; k != l; ++k)
					if (toolsString::equal(temp[1], k->first))
					{
						if (temp.size() == 3)
						{
							unsigned long pos = toolsString::stringToUL(temp[2]);
							if (pos >= 0 && pos <= k->second.size())
								return dodoString(1, k->second[pos]);
							else
								return __dodostring__;
						}
						else
							return k->second;
					}
			}

		dodoMap<dodoString, dodoStringArray>::iterator o(globalArray.begin()), p(globalArray.end());
		for (; o != p; ++o)
			if (toolsString::equal(temp[0], o->first))
			{
				unsigned long pos = toolsString::stringToUL(temp[1]);
				if (pos >= 0 && pos <= o->second.size())
				{
					if (temp.size() == 3)
					{
						unsigned long pos1 = toolsString::stringToUL(temp[2]);
						if (pos >= 0 && pos1 <= o->second[pos].size())
							return dodoString(1, o->second[pos][pos1]);
						else
							return __dodostring__;
					}
					else
						return o->second[pos];
				}
			}

		if (temp.size() >= 3)
		{
			dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d(globalArrayHash.begin()), f(globalArrayHash.end());
			for (; d != f; ++d)
				if (toolsString::equal(temp[0], d->first))
				{
					unsigned long pos = toolsString::stringToUL(temp[1]);
					if (pos >= 0 && pos <= d->second.size())
					{
						k = d->second[pos].begin();
						l = d->second[pos].end();
						for (; k != l; ++k)
							if (toolsString::equal(temp[2], k->first))
							{
								if (temp.size() == 4)
								{
									pos = toolsString::stringToUL(temp[3]);
									if (pos >= 0 && pos <= k->second.size())
										return dodoString(1, k->second[pos]);
									else
										return __dodostring__;
								}
								else
									return k->second;
							}
					}
				}
		}
	}

	return __dodostring__;
}

//-------------------------------------------------------------------

dodoString
cgiProcessor::trim(const dodoString &statement)
{
	dodoString temp = toolsString::trim(statement, " \t\n", 3);

	unsigned long i(temp.size() - 1);

	if (temp[0] == '\"' && temp[i] == '\"')
	{
		temp.erase(i);
		temp.erase(0, 1);
	}
	else
	{
		if (temp[0] == '\'' && temp[i] == '\'')
		{
			temp.erase(i);
			temp.erase(0, 1);
		}
		else
		{
			if (temp[0] == '`' && temp[i] == '`')
			{
				temp.erase(i);
				temp.erase(0, 1);
			}
		}
	}

	return temp;
}

//-------------------------------------------------------------------

void
cgiProcessor::display(const dodoString &path)
{
	CGI.print(this->process(path));
	CGI.flush();
}

//-------------------------------------------------------------------

