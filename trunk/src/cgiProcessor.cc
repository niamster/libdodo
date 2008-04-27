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

using namespace dodo::cgi;

processor::processor(client &a_cgi) : continueFlag(false),
										 breakDeepness(0),
										 loopDeepness(0),
										 iterator(1),
										 namespaceDeepness(1),
										 CGI(a_cgi)
{
	dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_VERSION]] = PACKAGE_STRING;
	dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = "1";
}

//-------------------------------------------------------------------

processor::~processor()
{
}

//-------------------------------------------------------------------

dodo::dodoString
processor::processString(const dodo::dodoString &tpl)
{
	dodo::dodoString tmp = _processString(preProcessString(tpl), "memory");

	newLinePositions.pop_back();

	return tmp;
}

//-------------------------------------------------------------------

dodo::dodoString
processor::process(const dodo::dodoString &path)
{
	dodo::dodoString tmp = _processString(preProcess(path), path);

	newLinePositions.pop_back();

	return tmp;
}

//-------------------------------------------------------------------

void
processor::clear()
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

dodo::dodoString
processor::_processString(const dodo::dodoString &buffer,
							 const dodo::dodoString &path)
{
	unsigned long i(0), j(0), begin(0), k(0);

	dodo::dodoString tpl;
	dodo::dodoString temp;

	while (true)
	{
		begin = j;

		i = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST], begin);
		if (i == dodo::dodoString::npos)
		{
			tpl.append(buffer.substr(begin));
			break;
		}
		else
			tpl.append(buffer.substr(begin, i - begin));

		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NP], i);

			++i;
			tpl.append(buffer.substr(i, j - i));
			j += 3;

			continue;
		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], i);

			j += 3;

			continue;
		}

		j = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], i);

		temp = buffer.substr(i, j - i);

		j += 2;

		k = temp.find(statements[PREPROCESSOR_PREPROCESSOR_PROCESSORSTATEMENT_INCLUDE]);
		if (k != dodo::dodoString::npos)
		{
			j = _include(j, temp.substr(k + 8), tpl, path);
		}
		else
		{
			k = temp.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF]);
			if (k != dodo::dodoString::npos)
			{
				++namespaceDeepness;

				j = _if(buffer, j, temp.substr(k + 2), tpl, path);

				cleanNamespace();

				--namespaceDeepness;
			}
			else
			{
				k = temp.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_PRINT]);
				if (k != dodo::dodoString::npos)
				{
					j = _print(j, temp.substr(k + 5), tpl, path);
				}
				else
				{
					k = temp.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_FOR]);
					if (k != dodo::dodoString::npos)
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
						k = temp.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_BREAK]);
						if (k != dodo::dodoString::npos)
						{
							if (_break(j, temp.substr(k + 5), path))
								break;
						}
						else
						{
							k = temp.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CONT]);
							if (k != dodo::dodoString::npos)
							{
								if (loopDeepness > 0)
								{
									continueFlag = true;
									break;
								}
							}
							else
							{
								k = temp.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_ASSIGN]);
								if (k != dodo::dodoString::npos)
								{
									j = _assign(j, temp.substr(k + 6), path);
								}
								else
								{
									k = temp.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_NS]);
									if (k != dodo::dodoString::npos)
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
processor::recursive(const dodo::dodoString &path)
{
	dodoList<dodo::dodoString>::iterator i(processed.begin()), j(processed.end());
	for (; i != j; ++i)
		if (toolsString::equal(*i, path))
			return true;

	return false;
}

//-------------------------------------------------------------------

void
processor::assign(dodo::dodoString varName,
					 const dodoArray<dodo::dodoStringMap> &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[PREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, PROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalArray.erase(varName);
	globalHash.erase(varName);

	globalArrayHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodo::dodoString varName,
					 const dodo::dodoStringMap &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[PREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, PROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalArray.erase(varName);
	globalArrayHash.erase(varName);

	globalHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodo::dodoString varName,
					 const dodo::dodoStringArray &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[PREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, PROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);

	globalArray[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodo::dodoString varName,
					 const dodo::dodoString &varVal)
{
	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[PREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, PROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	globalArray.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);

	global[varName] = varVal;
}

//-------------------------------------------------------------------

unsigned long
processor::_if(const dodo::dodoString &buffer,
				  unsigned long start,
				  const dodo::dodoString &statement,
				  dodo::dodoString &tpl,
				  const dodo::dodoString &path)
{
	bool _float(false), invert(false);

	unsigned short oper(0);

	dodoArray<dodo::dodoString> temp2 = tools::explode(statement, statements[PREPROCESSOR_PROCESSORSTATEMENT_EQ]);
	if (temp2.size() != 2)
	{
		temp2 = tools::explode(statement, statements[PREPROCESSOR_PROCESSORSTATEMENT_NE]);
		if (temp2.size() != 2)
		{
			temp2 = tools::explode(statement, statements[PREPROCESSOR_PROCESSORSTATEMENT_LE]);
			if (temp2.size() != 2)
			{
				temp2 = tools::explode(statement, statements[PREPROCESSOR_PROCESSORSTATEMENT_GE]);
				if (temp2.size() != 2)
				{
					temp2 = tools::explode(statement, statements[PREPROCESSOR_PROCESSORSTATEMENT_LT]);
					if (temp2.size() != 2)
					{
						temp2 = tools::explode(statement, statements[PREPROCESSOR_PROCESSORSTATEMENT_GT]);
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
			throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX__IF, ERR_LIBDODO, PROCESSOREX_WRONGIFSTATEMENT, PROCESSOREX_WRONGIFSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

		dodo::dodoString temp1 = toolsString::trim(temp2[0], " \t\n", 3);

		if (temp1[0] == '!')
		{
			invert = true;
			temp1 = temp1.substr(1);
		}

		temp1 = getVar(temp1, start, path);

		if (!toolsString::equal(temp1, statements[PREPROCESSOR_PROCESSORSTATEMENT_FALSE]) && temp1.size() != 0)
			accept = !invert;
		else
			accept = invert;
	}
	else
	{
		dodo::dodoString temp1 = getVar(temp2[0], start, path);

		dodo::dodoString temp3 = getVar(temp2[1], start, path);

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

	unsigned long u(blockEnd(buffer, start, statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF], statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_IF], path)), v(0);
	bool found(true);

	try
	{
		v = blockEnd(buffer, start, statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF], statements[PREPROCESSOR_PROCESSORSTATEMENT_ELSE], path);
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
			v = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], v) + 2;
			tpl.append(_processString(buffer.substr(v, u - v), path));
		}
	}

	return buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
processor::blockEnd(const dodo::dodoString &buffer,
					   unsigned long start,
					   const dodo::dodoString &st,
					   const dodo::dodoString &ts,
					   const dodo::dodoString &path)
{
	unsigned long u, m(start), _st(1), b, p, stLen(st.size()), tsLen(ts.size());

	while (true)
	{
		u = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST], m);
		if (u == dodo::dodoString::npos)
			throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_BLOCKEND, ERR_LIBDODO, PROCESSOREX_WRONGBLOCK, PROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

		b = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u + 2);
		if (b == dodo::dodoString::npos)
			throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_BLOCKEND, ERR_LIBDODO, PROCESSOREX_WRONGBLOCK, PROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

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
processor::_include(unsigned long start,
					const dodo::dodoString &statement,
					   dodo::dodoString &tpl,
					   const dodo::dodoString &path)
{
	dodo::dodoString temp1 = getVar(statement, start, path);

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
processor::_print(unsigned long start,
					const dodo::dodoString &statement,
					 dodo::dodoString &tpl,
					 const dodo::dodoString &path)
{
	dodo::dodoStringArray temp = tools::explode(statement, statements[PREPROCESSOR_PROCESSORSTATEMENT_COMA]);
	if (temp.size() == 1)
		tpl.append(getVar(statement, start, path));
	else
	{
		if (temp.size() > 1)
		{
			dodo::dodoStringArray::iterator i(temp.begin()), j(temp.end());
			for (; i != j; ++i)
				tpl.append(getVar(*i, start, path));
		}
	}
	
	return start;
}

//-------------------------------------------------------------------

bool
processor::_break(unsigned long start,
		 			const dodo::dodoString &statement,
					 const dodo::dodoString &path)
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
processor::_assign(unsigned long start,
						const dodo::dodoString &statement,
					  const dodo::dodoString &path)
{
	dodo::dodoStringArray temp = tools::explode(statement, statements[PREPROCESSOR_PREPROCESSOR_PROCESSORSTATEMENT_ASSIGN_OP], 2);

	if (temp.size() == 0)
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_WRONGASSIGNSTATEMENT, PROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodo::dodoString varName = trim(temp[0]);
	if (varName.size() == 0)
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_WRONGASSIGNSTATEMENT, PROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	if (varName[0] == '$')
		varName = varName.substr(1);

	if (toolsString::equal(varName, statements[PREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, PROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodo::dodoStringMap::iterator i = local.find(varName);
	if (i != local.end())
		if (localNamespace[namespaceDeepness].find(i->first) != localNamespace[namespaceDeepness].end())
			localNamespace[namespaceDeepness][i->first] = i->second;

	namespaceVars[namespaceDeepness].push_back(varName);
	local[varName] = getVar(temp[1], start, path);
	
	return start;
}

//-------------------------------------------------------------------

void
processor::cleanNamespace()
{
	dodoMap<unsigned int, dodo::dodoStringArray>::iterator c = namespaceVars.find(namespaceDeepness);
	if (c != namespaceVars.end())
	{
		dodoMap<unsigned int, dodo::dodoStringMap>::iterator v = localNamespace.find(namespaceDeepness);
		bool inLocal = v != localNamespace.end() ? true : false;

		dodo::dodoStringArray::iterator x(c->second.begin()), z(c->second.end());
		for (; x != z; ++x)
			if (inLocal)
			{
				dodo::dodoStringMap::iterator k = v->second.find(*x);
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
processor::_ns(const dodo::dodoString &buffer,
				  unsigned long start,
				  dodo::dodoString &tpl,
				  const dodo::dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_NS], statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NS], path));

	tpl.append(_processString(buffer.substr(start, u - start), path));

	return buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
processor::_for(const dodo::dodoString &buffer,
				   unsigned long start,
				   const dodo::dodoString &statement,
				   dodo::dodoString &tpl,
				   const dodo::dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_FOR], statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_FOR], path));

	unsigned long p = statement.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_DOLLAR]);
	unsigned long i(p), j(statement.size());

	for (; i < j; ++i)
		if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
			break;

	dodo::dodoString varName = statement.substr(p, i - p).substr(1);

	dodo::dodoString keyVal;
	dodo::dodoStringMap::iterator keyIter;
	bool key(false);
	dodo::dodoString keyName;

	p = statement.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_KEY_VALUE], i + 1);
	if (p != dodo::dodoString::npos)
	{
		key = true;
		p = statement.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_DOLLAR], p + 2);

		i = p;
		for (; i < j; ++i)
			if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
				break;

		keyName = varName;
		varName = statement.substr(p, i - p).substr(1);
	}

	p = statement.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_IN], i + 1);
	if (p == dodo::dodoString::npos)
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX__FOR, ERR_LIBDODO, PROCESSOREX_WRONGFORSTATEMENT, PROCESSOREX_WRONGFORSTATEMENT_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodo::dodoString targetVar = trim(statement.substr(p + 2));
	dodo::dodoString forSpace = buffer.substr(start, u - start);

	u = buffer.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], u) + 2;

	if (targetVar[0] == '$')
	{
		targetVar = targetVar.substr(1);

		dodo::dodoStringArray temp = tools::explode(targetVar, statements[PREPROCESSOR_PROCESSORSTATEMENT_DOT]);

		if (temp.size() == 1)
		{
			dodo::dodoStringMap::iterator k = local.begin();
			dodo::dodoStringMap::iterator l = local.end();
			for (; k != l; ++k)
				if (toolsString::equal(targetVar, k->first))
				{
					dodo::dodoStringMap::iterator iter = local.find(varName);
					dodo::dodoString iterVal;
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
						dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = toolsString::lToString(i);
						local[varName] = dodo::dodoString(1, k->second[i]);
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
					dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

			dodoMap<dodo::dodoString, dodo::dodoStringMap>::iterator g = localHash.begin();
			dodoMap<dodo::dodoString, dodo::dodoStringMap>::iterator h = localHash.end();
			for (; g != h; ++g)
				if (toolsString::equal(temp[0], g->first))
				{
					dodo::dodoStringMap::iterator iter = local.find(varName);
					dodo::dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodo::dodoStringMap::iterator k = g->second.begin();
					dodo::dodoStringMap::iterator l = g->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (; k != l; ++k, ++iterator)
					{
						dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

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
					dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
					dodo::dodoStringMap::iterator iter = local.find(varName);
					dodo::dodoString iterVal;
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
						dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

						if (key)
							local[keyName] = toolsString::lToString(i);
						local[varName] = dodo::dodoString(1, k->second[i]);
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
					dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
					dodo::dodoStringMap::iterator iter = local.find(varName);
					dodo::dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodo::dodoStringMap::iterator k = g->second.begin();
					dodo::dodoStringMap::iterator l = g->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (; k != l; ++k, ++iterator)
					{
						dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

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
					dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

			dodoMap<dodo::dodoString, dodo::dodoStringArray>::iterator o = globalArray.begin();
			dodoMap<dodo::dodoString, dodo::dodoStringArray>::iterator p = globalArray.end();
			for (; o != p; ++o)
				if (toolsString::equal(temp[0], o->first))
				{
					dodo::dodoStringMap::iterator iter = local.find(varName);
					dodo::dodoString iterVal;
					if (iter != local.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodo::dodoStringArray::iterator k = o->second.begin();
					dodo::dodoStringArray::iterator l = o->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (unsigned long keyNIter(0); k != l; ++k, ++keyNIter, ++iterator)
					{
						dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

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
					dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

			dodoMap<dodo::dodoString, dodoArray<dodo::dodoStringMap> >::iterator d = globalArrayHash.begin();
			dodoMap<dodo::dodoString, dodoArray<dodo::dodoStringMap> >::iterator f = globalArrayHash.end();
			for (; d != f; ++d)
				if (toolsString::equal(temp[0], d->first))
				{
					dodoMap<dodo::dodoString, dodo::dodoStringMap>::iterator iter = localHash.find(varName);
					dodo::dodoStringMap iterVal;
					if (iter != localHash.end())
						iterVal = iter->second;

					if (key)
					{
						keyIter = local.find(keyName);
						if (keyIter != local.end())
							keyVal = local[keyName];
					}

					dodoArray<dodo::dodoStringMap>::iterator k = d->second.begin();
					dodoArray<dodo::dodoStringMap>::iterator l = d->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (unsigned long keyNIter(0); k != l; ++k, ++keyNIter, ++iterator)
					{
						dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

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
					dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
				dodoMap<dodo::dodoString, dodo::dodoStringMap>::iterator g = localHash.begin();
				dodoMap<dodo::dodoString, dodo::dodoStringMap>::iterator h = localHash.end();
				for (; g != h; ++g)
					if (toolsString::equal(temp[0], g->first))
					{
						dodo::dodoStringMap::iterator k = g->second.begin();
						dodo::dodoStringMap::iterator l = g->second.end();
						for (; k != l; ++k)
							if (toolsString::equal(temp[1], k->first))
							{
								dodo::dodoStringMap::iterator iter = local.find(varName);
								dodo::dodoString iterVal;
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
									dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

									if (key)
										local[keyName] = toolsString::lToString(i);
									local[varName] = dodo::dodoString(1, k->second[i]);
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
								dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
						dodo::dodoStringMap::iterator k = g->second.begin();
						dodo::dodoStringMap::iterator l = g->second.end();
						for (; k != l; ++k)
							if (toolsString::equal(temp[1], k->first))
							{
								dodo::dodoStringMap::iterator iter = local.find(varName);
								dodo::dodoString iterVal;
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
									dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

									if (key)
										local[keyName] = toolsString::lToString(i);
									local[varName] = dodo::dodoString(1, k->second[i]);
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
								dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

				dodoMap<dodo::dodoString, dodo::dodoStringArray>::iterator o = globalArray.begin();
				dodoMap<dodo::dodoString, dodo::dodoStringArray>::iterator p = globalArray.end();
				for (; o != p; ++o)
					if (toolsString::equal(temp[0], o->first))
					{
						unsigned long pos = toolsString::stringToUL(temp[1]);
						if (pos >= 0 && pos <= o->second.size())
						{
							dodo::dodoStringMap::iterator iter = local.find(varName);
							dodo::dodoString iterVal;
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
								dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

								if (key)
									local[keyName] = toolsString::lToString(i);
								local[varName] = dodo::dodoString(1, o->second[pos][i]);
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
							dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

				dodoMap<dodo::dodoString, dodoArray<dodo::dodoStringMap> >::iterator d = globalArrayHash.begin();
				dodoMap<dodo::dodoString, dodoArray<dodo::dodoStringMap> >::iterator f = globalArrayHash.end();
				for (; d != f; ++d)
					if (toolsString::equal(temp[0], d->first))
					{
						unsigned long pos = toolsString::stringToUL(temp[1]);
						if (pos >= 0 && pos <= d->second.size())
						{
							dodo::dodoStringMap::iterator iter = local.find(varName);
							dodo::dodoString iterVal;
							if (iter != local.end())
								iterVal = iter->second;

							if (key)
							{
								keyIter = local.find(keyName);
								if (keyIter != local.end())
									keyVal = local[keyName];
							}

							dodo::dodoStringMap::iterator k = d->second[pos].begin();
							dodo::dodoStringMap::iterator l = d->second[pos].end();
							unsigned long iteratorPrev = iterator;
							iterator = 1;
							for (; k != l; ++k, ++iterator)
							{
								dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

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
							dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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
					dodoMap<dodo::dodoString, dodoArray<dodo::dodoStringMap> >::iterator d = globalArrayHash.begin();
					dodoMap<dodo::dodoString, dodoArray<dodo::dodoStringMap> >::iterator f = globalArrayHash.end();
					for (; d != f; ++d)
						if (toolsString::equal(temp[0], d->first))
						{
							unsigned long pos = toolsString::stringToUL(temp[1]);
							if (pos >= 0 && pos <= d->second.size())
							{
								dodo::dodoStringMap::iterator k = d->second[pos].begin();
								dodo::dodoStringMap::iterator l = d->second[pos].end();
								for (; k != l; ++k)
									if (toolsString::equal(temp[2], k->first))
									{
										dodo::dodoStringMap::iterator iter = local.find(varName);
										dodo::dodoString iterVal;
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
											dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

											if (key)
												local[keyName] = toolsString::lToString(i);
											local[varName] = dodo::dodoString(1, k->second[i]);
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
										dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

		dodo::dodoStringMap::iterator iter = local.find(varName);
		dodo::dodoString iterVal;
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
			dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iterator);

			if (key)
				local[keyName] = toolsString::lToString(i);
			local[varName] = dodo::dodoString(1, targetVar[i]);
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
		dodo[statements[PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR]] = toolsString::lToString(iteratorPrev);

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

dodo::dodoString
processor::getVar(const dodo::dodoString &a_varName,
					 unsigned long start,
					 const dodo::dodoString &path)
{
	dodo::dodoString varName = trim(a_varName), tempVar;
	unsigned long u, b, m(0), ob, cb, i, c;

	while (true)
	{
		ob = 1;
		cb = 0;

		u = varName.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_OPEN_VARPART], m);
		if (u == dodo::dodoString::npos)
			break;

		c = u;
		while (true)
		{
			b = varName.find(statements[PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_VARPART], c + 1);
			if (b == dodo::dodoString::npos)
				throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_GETVAR, ERR_LIBDODO, PROCESSOREX_WRONGVARSTATEMENT, PROCESSOREX_WRONGVARSTATEMENT_STR, __LINE__, __FILE__,toolsString::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

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

	dodo::dodoStringArray temp = tools::explode(varName, statements[PREPROCESSOR_PROCESSORSTATEMENT_DOT]);

	if (temp.size() == 1)
	{
		if (toolsString::equal(varName, statements[PREPROCESSOR_PROCESSORSTATEMENT_DODO]))
			return "cgi framework libdodo";

		dodo::dodoStringMap::iterator k(local.begin()), l(local.end());
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
		if (toolsString::equal(temp[0], statements[PREPROCESSOR_PROCESSORSTATEMENT_DODO]))
		{
			dodo::dodoStringMap::iterator k(local.begin()), l(local.end());
			for (; k != l; ++k)
				if (toolsString::equal(temp[1], k->first))
					if (temp.size() == 3)
					{
						unsigned long pos = toolsString::stringToUL(temp[2]);
						if (pos >= 0 && pos <= k->second.size())
							return dodo::dodoString(1, k->second[pos]);
						else
							return __dodostring__;
					}
					else
						return k->second;

			return __dodostring__;
		}

		dodo::dodoStringMap::iterator k(local.begin()), l(local.end());
		for (; k != l; ++k)
			if (toolsString::equal(temp[0], k->first))
			{
				unsigned long pos = toolsString::stringToUL(temp[1]);
				if (pos >= 0 && pos <= k->second.size())
					return dodo::dodoString(1, k->second[pos]);
				else
					return __dodostring__;
			}

		dodoMap<dodo::dodoString, dodo::dodoStringMap>::iterator g(localHash.begin()), h(localHash.end());
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
								return dodo::dodoString(1, k->second[pos]);
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
					return dodo::dodoString(1, k->second[pos]);
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
								return dodo::dodoString(1, k->second[pos]);
							else
								return __dodostring__;
						}
						else
							return k->second;
					}
			}

		dodoMap<dodo::dodoString, dodo::dodoStringArray>::iterator o(globalArray.begin()), p(globalArray.end());
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
							return dodo::dodoString(1, o->second[pos][pos1]);
						else
							return __dodostring__;
					}
					else
						return o->second[pos];
				}
			}

		if (temp.size() >= 3)
		{
			dodoMap<dodo::dodoString, dodoArray<dodo::dodoStringMap> >::iterator d(globalArrayHash.begin()), f(globalArrayHash.end());
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
										return dodo::dodoString(1, k->second[pos]);
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

dodo::dodoString
processor::trim(const dodo::dodoString &statement)
{
	dodo::dodoString temp = toolsString::trim(statement, " \t\n", 3);

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
processor::display(const dodo::dodoString &path)
{
	CGI.print(this->process(path));
	CGI.flush();
}

//-------------------------------------------------------------------

