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

const dodoString cgiProcessor::statements[] = {"dodo",
						"<(",
						")>",
						"<)>",
						"*)>",
						"if",
						"else",
						"fi",
						"for",
						"in",
						"=>",
						"rof",
						"print",
						"break",
						"continue",
						"assign",
						"=",
						"ns",
						"sn",
						"include",
						"iterator",
						"version",
						".",
						",",
						"$",
						"false",
						"==",
						"!=",
						"<=",
						">=",
						"<",
						">",
						"{",
						"}"};

//-------------------------------------------------------------------
//
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
	dodo[statements[VERSION]] = PACKAGE_STRING;
	dodo[statements[ITERATOR]] = "1";

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
	dodo[statements[VERSION]] = PACKAGE_STRING;
	dodo[statements[ITERATOR]] = "1";
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

		i = buffer.find(statements[OPEN_ST], begin);
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
			j = buffer.find(statements[CLOSE_NP], i);

			++i;
			tpl.append(buffer.substr(i, j - i));
			j += 3;

			continue;
		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[CLOSE_ST], i);

			j += 3;

			continue;
		}

		j = buffer.find(statements[CLOSE_ST], i);

		temp = buffer.substr(i, j - i);

		j += 2;

		k = temp.find(statements[INCLUDE]);
		if (k != dodoString::npos)
		{
			_include(temp.substr(k + 8), tpl, j, path);
		}
		else
		{
			k = temp.find(statements[OPEN_IF]);
			if (k != dodoString::npos)
			{
				++namespaceDeepness;

				j = _if(buffer, j, temp.substr(k + 2), tpl, path);

				cleanNamespace();

				--namespaceDeepness;
			}
			else
			{
				k = temp.find(statements[PRINT]);
				if (k != dodoString::npos)
				{
					_print(temp.substr(k + 5), tpl, j, path);
				}
				else
				{
					k = temp.find(statements[OPEN_FOR]);
					if (k != dodoString::npos)
					{
						++_loopDeepness;
						++namespaceDeepness;

						j = _for(buffer, j, temp.substr(k + 3), tpl, path);

						cleanNamespace();

						--namespaceDeepness;
						--_loopDeepness;
					}
					else
					{
						k = temp.find(statements[BREAK]);
						if (k != dodoString::npos)
						{
							if (_break(temp.substr(k + 5), j, path))
								break;
						}
						else
						{
							k = temp.find(statements[CONT]);
							if (k != dodoString::npos)
							{
								if (_loopDeepness > 0)
								{
									_continueFlag = true;
									break;
								}
							}
							else
							{
								k = temp.find(statements[ASSIGN]);
								if (k != dodoString::npos)
								{
									_assign(temp.substr(k + 6), j, path);
								}
								else
								{
									k = temp.find(statements[OPEN_NS]);
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
	for (; i != j; ++i)
		if (stringTools::equal(*i, path))
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

	if (stringTools::equal(varName, statements[DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR_ASSIGN, ERR_LIBDODO, CGIPREPROCESSOR_DODOISRESERVEDVARNAME, CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

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

	if (stringTools::equal(varName, statements[DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR_ASSIGN, ERR_LIBDODO, CGIPREPROCESSOR_DODOISRESERVEDVARNAME, CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

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

	if (stringTools::equal(varName, statements[DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR_ASSIGN, ERR_LIBDODO, CGIPREPROCESSOR_DODOISRESERVEDVARNAME, CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

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

	if (stringTools::equal(varName, statements[DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR_ASSIGN, ERR_LIBDODO, CGIPREPROCESSOR_DODOISRESERVEDVARNAME, CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

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

	dodoArray<dodoString> temp2 = tools::explode(statement, statements[EQ]);
	if (temp2.size() != 2)
	{
		temp2 = tools::explode(statement, statements[NE]);
		if (temp2.size() != 2)
		{
			temp2 = tools::explode(statement, statements[LE]);
			if (temp2.size() != 2)
			{
				temp2 = tools::explode(statement, statements[GE]);
				if (temp2.size() != 2)
				{
					temp2 = tools::explode(statement, statements[LT]);
					if (temp2.size() != 2)
					{
						temp2 = tools::explode(statement, statements[GT]);
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
		{
			char message[128];
			sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

			throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR__IF, ERR_LIBDODO, CGIPREPROCESSOR_WRONGIFSTATEMENT, CGIPREPROCESSOR_WRONGIFSTATEMENT_STR, __LINE__, __FILE__, message);
		}

		dodoString temp1 = stringTools::trim(temp2[0], " \t\n", 3);

		if (temp1[0] == '!')
		{
			invert = true;
			temp1 = temp1.substr(1);
		}

		temp1 = getVar(temp1, start, path);

		if (!stringTools::equal(temp1, statements[FALSE]) && temp1.size() != 0)
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
			double first(stringTools::stringToD(temp1)), second(stringTools::stringToD(temp3));

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
			if (stringTools::equal(temp1, temp3))
				accept = !invert;
		}
	}

	unsigned long u(blockEnd(buffer, start, statements[OPEN_IF], statements[CLOSE_IF], path)), v(0);
	bool found(true);

	try
	{
		v = blockEnd(buffer, start, statements[OPEN_IF], statements[ELSE], path);
	}
	catch (...)
	{
		found = false;
	}

	if (accept)
	{
		if (!found)
			v = u;

		tpl.append(_process(buffer.substr(start, v - start), path));
	}
	else
	{
		if (found)
		{
			v = buffer.find(statements[CLOSE_ST], v) + 2;
			tpl.append(_process(buffer.substr(v, u - v), path));
		}
	}

	return buffer.find(statements[CLOSE_ST], u) + 2;
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
		u = buffer.find(statements[OPEN_ST], m);
		if (u == dodoString::npos)
		{
			char message[128];
			sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

			throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR_BLOCKEND, ERR_LIBDODO, CGIPREPROCESSOR_WRONGBLOCK, CGIPREPROCESSOR_WRONGBLOCK_STR, __LINE__, __FILE__, message);
		}

		b = buffer.find(statements[CLOSE_ST], u + 2);
		if (b == dodoString::npos)
		{
			char message[128];
			sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

			throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR_BLOCKEND, ERR_LIBDODO, CGIPREPROCESSOR_WRONGBLOCK, CGIPREPROCESSOR_WRONGBLOCK_STR, __LINE__, __FILE__, message);
		}

		for (p = u; p < b; ++p)
			if (buffer[p] != ' ' && buffer[p] != '\t' && buffer[p] != '\n')
			{
				if (stringTools::equal(buffer.substr(p, tsLen), ts))
					--_st;
				else
				{
					if (stringTools::equal(buffer.substr(p, stLen), st))
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
	dodoString temp1 = getVar(statement, start, path);

	if (!stringTools::equal(temp1, path) && !recursive(temp1))
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
	dodoStringArr temp = tools::explode(statement, statements[COMA]);
	if (temp.size() == 1)
		tpl.append(getVar(statement, start, path));
	else
	{
		if (temp.size() > 1)
		{
			dodoStringArr::iterator i(temp.begin()), j(temp.end());
			for (; i != j; ++i)
				tpl.append(getVar(*i, start, path));
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
		_breakDeepness = getVar(statement, start, path).toUL();

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
	dodoStringArr temp = tools::explode(statement, statements[ASSIGN_OP], 2);

	if (temp.size() == 0)
	{
		char message[128];
		sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR__ASSIGN, ERR_LIBDODO, CGIPREPROCESSOR_WRONGASSIGNSTATEMENT, CGIPREPROCESSOR_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, message);
	}

	dodoString varName = trim(temp[0]);
	if (varName.size() == 0)
	{
		char message[128];
		sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR__ASSIGN, ERR_LIBDODO, CGIPREPROCESSOR_WRONGASSIGNSTATEMENT, CGIPREPROCESSOR_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, message);
	}

	if (varName[0] == '$')
		varName = varName.substr(1);

	if (stringTools::equal(varName, statements[DODO]))
	{
		char message[128];
		sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR__ASSIGN, ERR_LIBDODO, CGIPREPROCESSOR_DODOISRESERVEDVARNAME, CGIPREPROCESSOR_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__, message);
	}

	dodoStringMap::iterator i = local.find(varName);
	if (i != local.end())
		if (localNamespace[namespaceDeepness].find(i->first) != localNamespace[namespaceDeepness].end())
			localNamespace[namespaceDeepness][i->first] = i->second;

	namespaceVars[namespaceDeepness].push_back(varName);
	local[varName] = getVar(temp[1], start, path);
}

//-------------------------------------------------------------------

void
cgiProcessor::cleanNamespace()
{
	std::map<unsigned int, dodoStringArr>::iterator c = namespaceVars.find(namespaceDeepness);
	if (c != namespaceVars.end())
	{
		std::map<unsigned int, dodoStringMap>::iterator v = localNamespace.find(namespaceDeepness);
		bool inLocal = v != localNamespace.end() ? true : false;

		dodoStringArr::iterator x(c->second.begin()), z(c->second.end());
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
	unsigned long u(blockEnd(buffer, start, statements[OPEN_NS], statements[CLOSE_NS], path));

	tpl.append(_process(buffer.substr(start, u - start), path));

	return buffer.find(statements[CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
cgiProcessor::_for(const dodoString &buffer,
				   unsigned long start,
				   const dodoString &statement,
				   dodoString &tpl,
				   const dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[OPEN_FOR], statements[CLOSE_FOR], path));

	unsigned long p = statement.find(statements[DOLLAR]);
	unsigned long i(p), j(statement.size());

	for (; i < j; ++i)
		if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
			break;

	dodoString varName = statement.substr(p, i - p).substr(1);

	dodoString keyVal;
	dodoStringMap::iterator keyIter;
	bool key(false);
	dodoString keyName;

	p = statement.find(statements[KEY_VALUE], i + 1);
	if (p != dodoString::npos)
	{
		key = true;
		p = statement.find(statements[DOLLAR], p + 2);

		i = p;
		for (; i < j; ++i)
			if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
				break;

		keyName = varName;
		varName = statement.substr(p, i - p).substr(1);
	}

	p = statement.find(statements[IN], i + 1);
	if (p == dodoString::npos)
	{
		char message[128];
		sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

		throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR__FOR, ERR_LIBDODO, CGIPREPROCESSOR_WRONGFORSTATEMENT, CGIPREPROCESSOR_WRONGFORSTATEMENT_STR, __LINE__, __FILE__, message);
	}

	dodoString targetVar = trim(statement.substr(p + 2));
	dodoString forSpace = buffer.substr(start, u - start);

	u = buffer.find(statements[CLOSE_ST], u) + 2;

	if (targetVar[0] == '$')
	{
		targetVar = targetVar.substr(1);

		dodoStringArr temp = tools::explode(targetVar, statements[DOT]);

		if (temp.size() == 1)
		{
			dodoStringMap::iterator k = local.begin();
			dodoStringMap::iterator l = local.end();
			for (; k != l; ++k)
				if (stringTools::equal(targetVar, k->first))
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
						dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

						if (key)
							local[keyName] = stringTools::lToString(i);
						local[varName] = dodoString(1, k->second[i]);
						tpl.append(_process(forSpace, path));

						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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

			std::map<dodoString, dodoStringMap>::iterator g = localHash.begin();
			std::map<dodoString, dodoStringMap>::iterator h = localHash.end();
			for (; g != h; ++g)
				if (stringTools::equal(temp[0], g->first))
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
						dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

						if (key)
							local[keyName] = k->first;
						local[varName] = k->second;
						tpl.append(_process(forSpace, path));

						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
				if (stringTools::equal(targetVar, k->first))
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
						dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

						if (key)
							local[keyName] = stringTools::lToString(i);
						local[varName] = dodoString(1, k->second[i]);
						tpl.append(_process(forSpace, path));

						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
				if (stringTools::equal(temp[0], g->first))
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
						dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

						if (key)
							local[keyName] = k->first;
						local[varName] = k->second;
						tpl.append(_process(forSpace, path));

						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
			for (; o != p; ++o)
				if (stringTools::equal(temp[0], o->first))
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

					dodoStringArr::iterator k = o->second.begin();
					dodoStringArr::iterator l = o->second.end();
					unsigned long iteratorPrev = iterator;
					iterator = 1;
					for (unsigned long keyNIter(0); k != l; ++k, ++keyNIter, ++iterator)
					{
						dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

						if (key)
							local[keyName] = stringTools::lToString(keyNIter);
						local[varName] = *k;
						tpl.append(_process(forSpace, path));

						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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

			std::map<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.begin();
			std::map<dodoString, dodoArray<dodoStringMap> >::iterator f = globalArrayHash.end();
			for (; d != f; ++d)
				if (stringTools::equal(temp[0], d->first))
				{
					std::map<dodoString, dodoStringMap>::iterator iter = localHash.find(varName);
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
						dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

						if (key)
							local[keyName] = stringTools::lToString(keyNIter);
						localHash[varName] = *k;

						tpl.append(_process(forSpace, path));

						if (_breakDeepness > 0)
						{
							--_breakDeepness;
							break;
						}
						if (_continueFlag)
							_continueFlag = false;
					}

					iterator =  iteratorPrev;
					dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
				std::map<dodoString, dodoStringMap>::iterator g = localHash.begin();
				std::map<dodoString, dodoStringMap>::iterator h = localHash.end();
				for (; g != h; ++g)
					if (stringTools::equal(temp[0], g->first))
					{
						dodoStringMap::iterator k = g->second.begin();
						dodoStringMap::iterator l = g->second.end();
						for (; k != l; ++k)
							if (stringTools::equal(temp[1], k->first))
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
									dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

									if (key)
										local[keyName] = stringTools::lToString(i);
									local[varName] = dodoString(1, k->second[i]);
									tpl.append(_process(forSpace, path));

									if (_breakDeepness > 0)
									{
										--_breakDeepness;
										break;
									}
									if (_continueFlag)
										_continueFlag = false;
								}

								iterator =  iteratorPrev;
								dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
					if (stringTools::equal(temp[0], g->first))
					{
						dodoStringMap::iterator k = g->second.begin();
						dodoStringMap::iterator l = g->second.end();
						for (; k != l; ++k)
							if (stringTools::equal(temp[1], k->first))
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
									dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

									if (key)
										local[keyName] = stringTools::lToString(i);
									local[varName] = dodoString(1, k->second[i]);
									tpl.append(_process(forSpace, path));

									if (_breakDeepness > 0)
									{
										--_breakDeepness;
										break;
									}
									if (_continueFlag)
										_continueFlag = false;
								}

								iterator =  iteratorPrev;
								dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
				for (; o != p; ++o)
					if (stringTools::equal(temp[0], o->first))
					{
						unsigned long pos = stringTools::stringToUL(temp[1]);
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
								dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

								if (key)
									local[keyName] = stringTools::lToString(i);
								local[varName] = dodoString(1, o->second[pos][i]);
								tpl.append(_process(forSpace, path));

								if (_breakDeepness > 0)
								{
									--_breakDeepness;
									break;
								}
								if (_continueFlag)
									_continueFlag = false;
							}

							iterator =  iteratorPrev;
							dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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

				std::map<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.begin();
				std::map<dodoString, dodoArray<dodoStringMap> >::iterator f = globalArrayHash.end();
				for (; d != f; ++d)
					if (stringTools::equal(temp[0], d->first))
					{
						unsigned long pos = stringTools::stringToUL(temp[1]);
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
								dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

								if (key)
									local[keyName] = k->first;
								local[varName] = k->second;
								tpl.append(_process(forSpace, path));

								if (_breakDeepness > 0)
								{
									--_breakDeepness;
									break;
								}
								if (_continueFlag)
									_continueFlag = false;
							}

							iterator =  iteratorPrev;
							dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
					std::map<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.begin();
					std::map<dodoString, dodoArray<dodoStringMap> >::iterator f = globalArrayHash.end();
					for (; d != f; ++d)
						if (stringTools::equal(temp[0], d->first))
						{
							unsigned long pos = stringTools::stringToUL(temp[1]);
							if (pos >= 0 && pos <= d->second.size())
							{
								dodoStringMap::iterator k = d->second[pos].begin();
								dodoStringMap::iterator l = d->second[pos].end();
								for (; k != l; ++k)
									if (stringTools::equal(temp[2], k->first))
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
											dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

											if (key)
												local[keyName] = stringTools::lToString(i);
											local[varName] = dodoString(1, k->second[i]);
											tpl.append(_process(forSpace, path));

											if (_breakDeepness > 0)
											{
												--_breakDeepness;
												break;
											}
											if (_continueFlag)
												_continueFlag = false;
										}

										iterator =  iteratorPrev;
										dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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
			dodo[statements[ITERATOR]] = stringTools::lToString(iterator);

			if (key)
				local[keyName] = stringTools::lToString(i);
			local[varName] = dodoString(1, targetVar[i]);
			tpl.append(_process(forSpace, path));

			if (_breakDeepness > 0)
			{
				--_breakDeepness;
				break;
			}
			if (_continueFlag)
				_continueFlag = false;
		}

		iterator =  iteratorPrev;
		dodo[statements[ITERATOR]] = stringTools::lToString(iteratorPrev);

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

		u = varName.find(statements[OPEN_VARPART], m);
		if (u == dodoString::npos)
			break;

		c = u;
		while (true)
		{
			b = varName.find(statements[CLOSE_VARPART], c + 1);
			if (b == dodoString::npos)
			{
				char message[128];
				sprintf(message, " Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str());

				throw baseEx(ERRMODULE_CGIPROCESSOR, CGIPROCESSOR_GETVAR, ERR_LIBDODO, CGIPREPROCESSOR_WRONGVARSTATEMENT, CGIPREPROCESSOR_WRONGVARSTATEMENT_STR, __LINE__, __FILE__, message);
			}

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

	dodoStringArr temp = tools::explode(varName, statements[DOT]);

	if (temp.size() == 1)
	{
		if (stringTools::equal(varName, statements[DODO]))
			return "cgi framework libdodo";

		dodoStringMap::iterator k(local.begin()), l(local.end());
		for (; k != l; ++k)
			if (stringTools::equal(varName, k->first))
				return k->second;

		k = global.begin();
		l = global.end();
		for (; k != l; ++k)
			if (stringTools::equal(varName, k->first))
				return k->second;
	}
	else
	{
		if (stringTools::equal(temp[0], statements[DODO]))
		{
			dodoStringMap::iterator k(local.begin()), l(local.end());
			for (; k != l; ++k)
				if (stringTools::equal(temp[1], k->first))
					if (temp.size() == 3)
					{
						unsigned long pos = stringTools::stringToUL(temp[2]);
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
			if (stringTools::equal(temp[0], k->first))
			{
				unsigned long pos = stringTools::stringToUL(temp[1]);
				if (pos >= 0 && pos <= k->second.size())
					return dodoString(1, k->second[pos]);
				else
					return __dodostring__;
			}

		std::map<dodoString, dodoStringMap>::iterator g(localHash.begin()), h(localHash.end());
		for (; g != h; ++g)
			if (stringTools::equal(temp[0], g->first))
			{
				k = g->second.begin();
				l = g->second.end();
				for (; k != l; ++k)
					if (stringTools::equal(temp[1], k->first))
					{
						if (temp.size() == 3)
						{
							unsigned long pos = stringTools::stringToUL(temp[2]);
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
			if (stringTools::equal(temp[0], k->first))
			{
				unsigned long pos = stringTools::stringToUL(temp[1]);
				if (pos >= 0 && pos <= k->second.size())
					return dodoString(1, k->second[pos]);
				else
					return __dodostring__;
			}

		g = globalHash.begin();
		h = globalHash.end();
		for (; g != h; ++g)
			if (stringTools::equal(temp[0], g->first))
			{
				k = g->second.begin();
				l = g->second.end();
				for (; k != l; ++k)
					if (stringTools::equal(temp[1], k->first))
					{
						if (temp.size() == 3)
						{
							unsigned long pos = stringTools::stringToUL(temp[2]);
							if (pos >= 0 && pos <= k->second.size())
								return dodoString(1, k->second[pos]);
							else
								return __dodostring__;
						}
						else
							return k->second;
					}
			}

		std::map<dodoString, dodoStringArr>::iterator o(globalArray.begin()), p(globalArray.end());
		for (; o != p; ++o)
			if (stringTools::equal(temp[0], o->first))
			{
				unsigned long pos = stringTools::stringToUL(temp[1]);
				if (pos >= 0 && pos <= o->second.size())
				{
					if (temp.size() == 3)
					{
						unsigned long pos1 = stringTools::stringToUL(temp[2]);
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
			std::map<dodoString, dodoArray<dodoStringMap> >::iterator d(globalArrayHash.begin()), f(globalArrayHash.end());
			for (; d != f; ++d)
				if (stringTools::equal(temp[0], d->first))
				{
					unsigned long pos = stringTools::stringToUL(temp[1]);
					if (pos >= 0 && pos <= d->second.size())
					{
						k = d->second[pos].begin();
						l = d->second[pos].end();
						for (; k != l; ++k)
							if (stringTools::equal(temp[2], k->first))
							{
								if (temp.size() == 4)
								{
									pos = stringTools::stringToUL(temp[3]);
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
	dodoString temp = stringTools::trim(statement, " \t\n", 3);

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
