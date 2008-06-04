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

processor::processor(server &a_cgi) : continueFlag(false),
									  breakDeepness(0),
									  loopDeepness(0),
									  iterator(1),
									  namespaceDeepness(1),
									  CGI(a_cgi)
{
	dodo[statements[PREPROCESSOR_STATEMENT_VERSION]] = PACKAGE_STRING;
	dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = "1";
}

//-------------------------------------------------------------------

processor::~processor()
{
}

//-------------------------------------------------------------------

dodoString
processor::processString(const dodoString &tpl)
{
	dodoString tmp = _processString(preProcessString(tpl), "memory");

	newLinePositions.pop_back();

	return tmp;
}

//-------------------------------------------------------------------

dodoString
processor::process(const dodoString &path)
{
	dodoString tmp = _processString(preProcess(path), path);

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
}

//-------------------------------------------------------------------

dodoString
processor::_processString(const dodoString &buffer,
						  const dodoString &path)
{
	unsigned long i(0), j(0), begin(0), k(0);
	unsigned long stI;

	dodoString tpl;
	dodoString temp;

	bool breakLoop = false;
	bool keywordNotFound = false;

	while (true)
	{
		begin = j;

		i = buffer.find(statements[PREPROCESSOR_STATEMENT_OPEN_ST], begin);
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
			j = buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_NP], i);

			++i;
			tpl.append(buffer.substr(i, j - i));
			j += 3;

			continue;
		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_COMM], i);

			j += 3;

			continue;
		}

		j = buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_ST], i);

		for (stI=i; stI < j; ++stI)
			if (buffer[stI] != '\t' && buffer[stI] != ' ' && buffer[stI] != '\r' && buffer[stI] != '\n')
				break;

		temp = buffer.substr(stI, j - stI);

		j += 2;

		keywordNotFound = false;

		switch (temp[0])
		{
			case 'p':

				k = temp.find(statements[PREPROCESSOR_STATEMENT_PRINT]);
				if (k == 0)
					j = _print(j, temp.substr(5), tpl, path);
				else
					keywordNotFound = true;

				break;

			case 'i':

				k = temp.find(statements[PREPROCESSOR_STATEMENT_OPEN_IF]);
				if (k == 0)
				{
					++namespaceDeepness;

					j = _if(buffer, j, temp.substr(2), tpl, path);

					cleanNamespace();

					--namespaceDeepness;

				}
				else
				{
					k = temp.find(statements[PREPROCESSOR_STATEMENT_INCLUDE]);
					if (k == 0)
						j = _include(j, temp.substr(8), tpl, path);
					else
						keywordNotFound = true;
				}

				break;

			case 'f':

				k = temp.find(statements[PREPROCESSOR_STATEMENT_OPEN_FOR]);
				if (k == 0)
				{
					++loopDeepness;
					++namespaceDeepness;

					j = _for(buffer, j, temp.substr(3), tpl, path);

					cleanNamespace();

					--namespaceDeepness;
					--loopDeepness;
				}
				else
					keywordNotFound = true;

				break;

			case 'b':

				k = temp.find(statements[PREPROCESSOR_STATEMENT_BREAK]);
				if (k == 0)
				{
					if (_break(j, temp.substr(5), path))
						breakLoop = true;
				}
				else
					keywordNotFound = true;

				break;

			case 'c':
							
				k = temp.find(statements[PREPROCESSOR_STATEMENT_CONT]);
				if (k == 0)
				{
					if (loopDeepness > 0)
					{
						continueFlag = true;

						breakLoop = true;
					}
				}
				else
					keywordNotFound = true;

				break;

			case 'a':

				k = temp.find(statements[PREPROCESSOR_STATEMENT_ASSIGN]);
				if (k == 0)
					j = _assign(j, temp.substr(6), path);
				else
					keywordNotFound = true;
				
				break;

			case 'n':
				
				k = temp.find(statements[PREPROCESSOR_STATEMENT_OPEN_NS]);
				if (k == 0)
				{
					++namespaceDeepness;

					j = _ns(buffer, j, tpl, path);

					cleanNamespace();

					--namespaceDeepness;
				}
				else
					keywordNotFound = true;

				break;

			default:

				keywordNotFound = true;
		}

		if (keywordNotFound)
			tpl.append(buffer.substr(i - 2, j - i + 2));

		if (breakLoop)
			break;

		if (breakDeepness > 0)
			break;

		if (continueFlag)
			break;
	}

	return tpl;
}

//-------------------------------------------------------------------

bool
processor::recursive(const dodoString &path)
{
	dodoList<dodoString>::iterator i(processed.begin()), j(processed.end());
	for (; i != j; ++i)
		if (tools::string::equal(*i, path))
			return true;

	return false;
}

//-------------------------------------------------------------------

void
processor::assign(dodoString varName,
				  const dodoArray<dodoStringMap> &varVal)
{
	if (tools::string::equal(varName, statements[PREPROCESSOR_STATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalArray.erase(varName);
	globalHash.erase(varName);

	globalArrayHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodoString varName,
				  const dodoStringMap &varVal)
{
	if (tools::string::equal(varName, statements[PREPROCESSOR_STATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalArray.erase(varName);
	globalArrayHash.erase(varName);

	globalHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodoString varName,
				  const dodoStringArray &varVal)
{
	if (tools::string::equal(varName, statements[PREPROCESSOR_STATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	global.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);

	globalArray[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodoString varName,
				  const dodoString &varVal)
{
	if (tools::string::equal(varName, statements[PREPROCESSOR_STATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);

	globalArray.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);

	global[varName] = varVal;
}

//-------------------------------------------------------------------

unsigned long
processor::_if(const dodoString &buffer,
			   unsigned long start,
			   const dodoString &statement,
			   dodoString &tpl,
			   const dodoString &path)
{
	bool _float(false), invert(false);

	enum operTypeEnum
	{
		OPERTYPE_NONE,
		OPERTYPE_LE,
		OPERTYPE_GE,
		OPERTYPE_LT,
		OPERTYPE_GT
	};

	unsigned short oper(OPERTYPE_NONE);

	dodoString temp1;
	dodoArray<dodoString> temp2;
	unsigned long i = 0, j = statement.size();

	for (;i<j;++i)
	{
		if ( (j - i) > 1 )
		{
			if (statement[i] == '=' && statement[i + 1] == '=')
			{
				temp2.push_back(statement.substr(0, i));
				temp2.push_back(statement.substr(i + 2));

				break;
			}
			if (statement[i] == '!' && statement[i + 1] == '=')
			{
				invert = true;

				temp2.push_back(statement.substr(0, i));
				temp2.push_back(statement.substr(i + 2));

				break;
			}
			if (statement[i] == '>' && statement[i + 1] == '=')
			{
				oper = OPERTYPE_GE;
				_float = true;

				temp2.push_back(statement.substr(0, i));
				temp2.push_back(statement.substr(i + 2));

				break;
			}
			if (statement[i] == '<' && statement[i + 1] == '=')
			{
				oper = OPERTYPE_LE;
				_float = true;

				temp2.push_back(statement.substr(0, i));
				temp2.push_back(statement.substr(i + 2));

				break;
			}
		}
		if (statement[i] == '<')
		{
			oper = OPERTYPE_LT;
			_float = true;

			temp2.push_back(statement.substr(0, i));
			temp2.push_back(statement.substr(i + 1));

			break;
		}
		if (statement[i] == '>')
		{
			oper = OPERTYPE_GT;
			_float = true;

			temp2.push_back(statement.substr(0, i));
			temp2.push_back(statement.substr(i + 1));

			break;
		}
		if (statement[i] == '!')
		{
			invert = true;

			temp1 = statement.substr(i + 1);

			break;
		}
	}

	bool accept(invert);

	if (temp2.size() != 2)
	{
		if (!tools::string::equal(getVar(temp1, start, path), statements[PREPROCESSOR_STATEMENT_FALSE]))
			accept = !invert;
		else
			accept = invert;
	}
	else
	{
		temp1 = getVar(temp2[0], start, path);

		dodoString temp3 = getVar(temp2[1], start, path);

		if (_float)
		{
			double first(tools::string::stringToD(temp1)), second(tools::string::stringToD(temp3));

			switch (oper)
			{
				case OPERTYPE_LE:

					accept = (first <= second);

					break;

				case OPERTYPE_GE:

					accept = (first >= second);

					break;

				case OPERTYPE_LT:

					accept = (first < second);

					break;

				case OPERTYPE_GT:

					accept = (first > second);

					break;
			}
		}
		else
		{
			if (tools::string::equal(temp1, temp3))
				accept = !invert;
		}
	}

	unsigned long u(blockEnd(buffer, start, statements[PREPROCESSOR_STATEMENT_OPEN_IF], statements[PREPROCESSOR_STATEMENT_CLOSE_IF], path)), v(0);
	bool found(true);

	try
	{
		v = blockEnd(buffer, start, statements[PREPROCESSOR_STATEMENT_OPEN_IF], statements[PREPROCESSOR_STATEMENT_ELSE], path);
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
			v = buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_ST], v) + 2;

			tpl.append(_processString(buffer.substr(v, u - v), path));
		}
	}

	return buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
processor::blockEnd(const dodoString &buffer,
					unsigned long start,
					const dodoString &st,
					const dodoString &ts,
					const dodoString &path)
{
	unsigned long u, m(start), _st(1), b, p, stLen(st.size()), tsLen(ts.size());

	while (true)
	{
		u = buffer.find(statements[PREPROCESSOR_STATEMENT_OPEN_ST], m);
		if (u == dodoString::npos)
			throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_BLOCKEND, ERR_LIBDODO, PROCESSOREX_WRONGBLOCK, CGIPROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

		b = buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_ST], u + 2);
		if (b == dodoString::npos)
			throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_BLOCKEND, ERR_LIBDODO, PROCESSOREX_WRONGBLOCK, CGIPROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

		for (p = u; p < b; ++p)
			if (buffer[p] != ' ' && buffer[p] != '\t' && buffer[p] != '\n' && buffer[p] != '\r')
			{
				if (tools::string::equal(buffer.substr(p, tsLen), ts))
					--_st;
				else
				{
					if (tools::string::equal(buffer.substr(p, stLen), st))
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
					const dodoString &statement,
					dodoString &tpl,
					const dodoString &path)
{
	dodoString temp1 = getVar(statement, start, path);

	if (!tools::string::equal(temp1, path) && !recursive(temp1))
	{
		if (!tplBasePath.empty())
			temp1 = tplBasePath + FILE_DELIM + temp1;
		
		if (tools::filesystem::exists(temp1))
		{
			processed.push_back(path);

			tpl.append(process(temp1));

			processed.pop_back();
		}
	}

	return start;
}

//-------------------------------------------------------------------

unsigned long
processor::_print(unsigned long start,
				  const dodoString &statement,
				  dodoString &tpl,
				  const dodoString &path)
{
	dodoStringArray temp = tools::misc::explode(statement, statements[PREPROCESSOR_STATEMENT_COMA]);
	if (temp.size() <= 1)
		tpl.append(getVar(statement, start, path));
	else
	{
		dodoStringArray::iterator i(temp.begin()), j(temp.end());
		for (; i != j; ++i)
			tpl.append(getVar(*i, start, path));
	}

	return start;
}

//-------------------------------------------------------------------

bool
processor::_break(unsigned long start,
				  const dodoString &statement,
				  const dodoString &path)
{
	if (loopDeepness > 0)
	{
		breakDeepness = tools::string::stringToUL(getVar(statement, start, path));

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
				   const dodoString &statement,
				   const dodoString &path)
{
	dodoStringArray temp = tools::misc::explode(statement, statements[PREPROCESSOR_STATEMENT_ASSIGN_OP], 2);

	if (temp.size() == 0)
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX__ASSIGN, ERR_LIBDODO, PROCESSOREX_WRONGASSIGNSTATEMENT, CGIPROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodoString varName = trim(temp[0]);
	if (varName.size() == 0)
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX__ASSIGN, ERR_LIBDODO, PROCESSOREX_WRONGASSIGNSTATEMENT, CGIPROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	if (varName[0] == '$')
		varName = varName.substr(1);

	if (tools::string::equal(varName, statements[PREPROCESSOR_STATEMENT_DODO]))
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX__ASSIGN, ERR_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, CGIPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	local[namespaceDeepness][varName] = getVar(temp[1], start, path);

	return start;
}

//-------------------------------------------------------------------

void
processor::cleanNamespace()
{
	local.erase(namespaceDeepness);
	localHash.erase(namespaceDeepness);
}

//-------------------------------------------------------------------

unsigned long
processor::_ns(const dodoString &buffer,
			   unsigned long start,
			   dodoString &tpl,
			   const dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[PREPROCESSOR_STATEMENT_OPEN_NS], statements[PREPROCESSOR_STATEMENT_CLOSE_NS], path));

	tpl.append(_processString(buffer.substr(start, u - start), path));

	return buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
processor::_for(const dodoString &buffer,
				unsigned long start,
				const dodoString &statement,
				dodoString &tpl,
				const dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[PREPROCESSOR_STATEMENT_OPEN_FOR], statements[PREPROCESSOR_STATEMENT_CLOSE_FOR], path));

	unsigned long p = statement.find(statements[PREPROCESSOR_STATEMENT_DOLLAR]);
	unsigned long i(p), j(statement.size());

	for (; i < j; ++i)
		if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
			break;
	--i;

	dodoString varName = statement.substr(p + 1, i - p);

	bool key(false);
	dodoString keyName;

	p = statement.find(statements[PREPROCESSOR_STATEMENT_KEY_VALUE], i + 1);
	if (p != dodoString::npos)
	{
		key = true;
		p = statement.find(statements[PREPROCESSOR_STATEMENT_DOLLAR], p + 2);

		for (i=p; i < j; ++i)
			if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
				break;
		--i;

		keyName = varName;
		varName = statement.substr(p + 1, i - p);
	}

	p = statement.find(statements[PREPROCESSOR_STATEMENT_IN], i + 1);
	if (p == dodoString::npos)
		throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX__FOR, ERR_LIBDODO, PROCESSOREX_WRONGFORSTATEMENT, CGIPROCESSOREX_WRONGFORSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

	dodoString targetVar = getVarName(statement.substr(p + 2), start, path);
	dodoString forSpace = buffer.substr(start, u - start);

	u = buffer.find(statements[PREPROCESSOR_STATEMENT_CLOSE_ST], u) + 2;

	if (targetVar[0] == '$')
	{
		targetVar = targetVar.substr(1);

		dodoStringArray temp = tools::misc::explode(targetVar, statements[PREPROCESSOR_STATEMENT_DOT]);

		if (temp.size() == 1)
		{
			dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsi = localHash.rbegin();
			dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsj = localHash.rend();
			for (;lhpnsi!=lhpnsj;++lhpnsi)
			{
				dodoMap<dodoString, dodoStringMap>::iterator g = lhpnsi->second.find(targetVar);
				if (g != lhpnsi->second.end())
				{
					dodoStringMap &lns = local[namespaceDeepness];
					
					unsigned long iteratorPrev = iterator;
					iterator = 1;

					dodoStringMap::iterator k = g->second.begin();
					dodoStringMap::iterator l = g->second.end();
					for (; k != l; ++k, ++iterator)
					{
						dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

						if (key)
							lns[keyName] = k->first;
						lns[varName] = k->second;
						
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
					dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

					return u;
				}
			}
			
			dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsi = local.rbegin();
			dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsj = local.rend();
			for (;lpnsi!=lpnsj;++lpnsi)
			{
				dodoStringMap::iterator k = lpnsi->second.find(targetVar);
				if (k != lpnsi->second.end())
				{
					dodoStringMap &lns = local[namespaceDeepness];
					
					unsigned long iteratorPrev = iterator;
					iterator = 1;

					unsigned long i(0), j(k->second.size());
					for (; i < j; ++i, ++iterator)
					{
						dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

						if (key)
							lns[keyName] = tools::string::lToString(i);
						lns[varName] = dodoString(1, k->second[i]);
						
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
					dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);
					
					return u;
				}
			}

			dodoStringMap::iterator k = global.find(targetVar);
			if (k != global.end())
			{
				dodoStringMap &lns = local[namespaceDeepness];
				
				unsigned long iteratorPrev = iterator;
				iterator = 1;

				unsigned long i(0), j(k->second.size());
				for (; i < j; ++i, ++iterator)
				{
					dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
						lns[keyName] = tools::string::lToString(i);
					lns[varName] = dodoString(1, k->second[i]);
					
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
				dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

				return u;
			}

			dodoMap<dodoString, dodoStringMap>::iterator g = globalHash.find(targetVar);
			if (g != globalHash.end())
			{
				dodoStringMap &lns = local[namespaceDeepness];
			
				unsigned long iteratorPrev = iterator;
				iterator = 1;
				
				dodoStringMap::iterator k = g->second.begin();
				dodoStringMap::iterator l = g->second.end();
				for (; k != l; ++k, ++iterator)
				{
					dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
						lns[keyName] = k->first;
					lns[varName] = k->second;

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
				dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

				return u;
			}

			dodoMap<dodoString, dodoStringArray>::iterator o = globalArray.find(targetVar);
			if (o != globalArray.end())
			{
				dodoStringMap &lns = local[namespaceDeepness];
		
				unsigned long iteratorPrev = iterator;
				iterator = 1;
				
				dodoStringArray::iterator k = o->second.begin();
				dodoStringArray::iterator l = o->second.end();
				for (unsigned long keyNIter(0); k != l; ++k, ++keyNIter, ++iterator)
				{
					dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
						lns[keyName] = tools::string::lToString(keyNIter);
					lns[varName] = *k;

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
				dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

				return u;
			}

			dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.find(targetVar);
			if (d != globalArrayHash.end())
			{
				dodoStringMap &lns = local[namespaceDeepness];
				dodoMap<dodoString, dodoStringMap> &lnsh = localHash[namespaceDeepness];
				
				unsigned long iteratorPrev = iterator;
				iterator = 1;

				dodoArray<dodoStringMap>::iterator k = d->second.begin();
				dodoArray<dodoStringMap>::iterator l = d->second.end();
				for (unsigned long keyNIter(0); k != l; ++k, ++keyNIter, ++iterator)
				{
					dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
						lns[keyName] = tools::string::lToString(keyNIter);
					lnsh[varName] = *k;

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
				dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

				return u;
			}
		}
		else
		{
			if (temp.size() == 2)
			{
				dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsi = localHash.rbegin();
				dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsj = localHash.rend();
				for (;lhpnsi!=lhpnsj;++lhpnsi)
				{
					dodoMap<dodoString, dodoStringMap>::iterator g = lhpnsi->second.find(targetVar);
					if (g != lhpnsi->second.end())
					{
						dodoStringMap::iterator k = g->second.find(temp[1]);
						if (k != g->second.end())
						{
							dodoStringMap &lns = local[namespaceDeepness];
							
							unsigned long iteratorPrev = iterator;
							iterator = 1;

							unsigned long i(0), j(k->second.size());
							for (; i < j; ++i, ++iterator)
							{
								dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

								if (key)
									lns[keyName] = tools::string::lToString(i);
								lns[varName] = dodoString(1, k->second[i]);
						
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
							dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

							return u;
						}
					}
				}

				dodoMap<dodoString, dodoStringMap>::iterator g = globalHash.find(temp[0]);
				if (g != globalHash.end())
				{
					dodoStringMap::iterator k = g->second.find(temp[1]);
					if (k != g->second.end())
					{
						dodoStringMap &lns = local[namespaceDeepness];
						
						unsigned long iteratorPrev = iterator;
						iterator = 1;

						unsigned long i(0), j(k->second.size());
						for (; i < j; ++i, ++iterator)
						{
							dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

							if (key)
								lns[keyName] = tools::string::lToString(i);
							lns[varName] = dodoString(1, k->second[i]);
							
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
						dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

						return u;
					}
				}

				dodoMap<dodoString, dodoStringArray>::iterator o = globalArray.find(temp[0]);
				if (o != globalArray.end())
				{
					unsigned long pos = tools::string::stringToUL(temp[1]);
					if (pos >= 0 && pos <= o->second.size())
					{
						dodoStringMap &lns = local[namespaceDeepness];
						
						unsigned long iteratorPrev = iterator;
						iterator = 1;

						unsigned long i(0), j(o->second[pos].size());
						for (; i < j; ++i, ++iterator)
						{
							dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

							if (key)
								lns[keyName] = tools::string::lToString(i);
							lns[varName] = dodoString(1, o->second[pos][i]);
							
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
						dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

						return u;
					}
				}

				dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.find(temp[0]);
				if (d != globalArrayHash.end())
				{
					unsigned long pos = tools::string::stringToUL(temp[1]);
					if (pos >= 0 && pos <= d->second.size())
					{
						dodoStringMap &lns = local[namespaceDeepness];
						
						unsigned long iteratorPrev = iterator;
						iterator = 1;

						dodoStringMap::iterator k = d->second[pos].begin();
						dodoStringMap::iterator l = d->second[pos].end();
						for (; k != l; ++k, ++iterator)
						{
							dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

							if (key)
								lns[keyName] = k->first;
							lns[varName] = k->second;

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
						dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

						return u;
					}
				}
			}
			else
			{
				if (temp.size() == 3)
				{
					dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.find(temp[0]);
					if (d != globalArrayHash.end())
					{
						unsigned long pos = tools::string::stringToUL(temp[1]);
						if (pos >= 0 && pos <= d->second.size())
						{
							dodoStringMap::iterator k = d->second[pos].find(temp[2]);
							if (k != d->second[pos].end())
							{
								dodoStringMap &lns = local[namespaceDeepness];

								unsigned long iteratorPrev = iterator;
								iterator = 1;
								
								unsigned long i(0), j(k->second.size());
								for (; i < j; ++i, ++iterator)
								{
									dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

									if (key)
										lns[keyName] = tools::string::lToString(i);
									lns[varName] = dodoString(1, k->second[i]);
									
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
								dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
		dodoStringMap &lns = local[namespaceDeepness];
		
		unsigned long iteratorPrev = iterator;
		iterator = 1;

		unsigned long i(0), j(targetVar.size());
		for (; i < j; ++i, ++iterator)
		{
			dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

			if (key)
				lns[keyName] = tools::string::lToString(i);
			lns[varName] = dodoString(1, targetVar[i]);
			
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
		dodo[statements[PREPROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

		return u;
	}

	return u;
}

//-------------------------------------------------------------------

dodoString
processor::getVarName(const dodoString &a_varName,
				  unsigned long start,
				  const dodoString &path)
{
	dodoString varName = trim(a_varName), tempVar;

	unsigned long u, b, m(0), ob, cb, i, c;

	while (true)
	{
		ob = 1;
		cb = 0;

		u = varName.find(statements[PREPROCESSOR_STATEMENT_OPEN_VARPART], m);
		if (u == dodoString::npos)
			break;

		c = u;
		while (true)
		{
			b = varName.find(statements[PREPROCESSOR_STATEMENT_CLOSE_VARPART], c + 1);
			if (b == dodoString::npos)
				throw baseEx(ERRMODULE_CGIPROCESSOR, PROCESSOREX_GETVARNAME, ERR_LIBDODO, PROCESSOREX_WRONGVARSTATEMENT, CGIPROCESSOREX_WRONGVARSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));

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

	return varName;
}

//-------------------------------------------------------------------

dodoString
processor::getVar(const dodoString &a_varName,
				  unsigned long start,
				  const dodoString &path)
{
	dodoString varName = getVarName(a_varName, start, path);

	if (varName[0] != '$')
		return varName;
	else
		varName.erase(0, 1);

	dodoStringArray temp = tools::misc::explode(varName, statements[PREPROCESSOR_STATEMENT_DOT]);

	if (temp.size() == 1)
	{
		if (tools::string::equal(varName, statements[PREPROCESSOR_STATEMENT_DODO]))
			return "cgi framework libdodo";

		dodoStringMap::iterator k;

		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsi = local.rbegin();
		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsj = local.rend();
		for (;lpnsi!=lpnsj;++lpnsi)
		{
			k = lpnsi->second.find(varName);
			if (k != lpnsi->second.end())
				return k->second;
		}

		k = global.find(varName);
		if (k != global.end())
			return k->second;

		return __dodostring__;
	}
	else
	{
		if (tools::string::equal(temp[0], statements[PREPROCESSOR_STATEMENT_DODO]))
		{
			dodoStringMap::iterator k = dodo.find(temp[1]);
			if (k != dodo.end())
			{
				if (temp.size() == 3)
				{
					unsigned long pos = tools::string::stringToUL(temp[2]);
					if (pos >= 0 && pos <= k->second.size())
						return dodoString(1, k->second[pos]);
					else
						return __dodostring__;
				}
				else
					return k->second;
			}

			return __dodostring__;
		}

		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsi = local.rbegin();
		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsj = local.rend();
		for (;lpnsi!=lpnsj;++lpnsi)
		{
			dodoStringMap::iterator k = lpnsi->second.find(temp[0]);
			if (k != lpnsi->second.end())
			{
				unsigned long pos = tools::string::stringToUL(temp[1]);
				if (pos >= 0 && pos <= k->second.size())
					return dodoString(1, k->second[pos]);
				else
					return __dodostring__;
			}
		}

		dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsi = localHash.rbegin();
		dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsj = localHash.rend();
		for (;lhpnsi!=lhpnsj;++lhpnsi)
		{
			dodoMap<dodoString, dodoStringMap>::iterator g = lhpnsi->second.find(temp[0]);
			if (g != lhpnsi->second.end())
			{
				dodoStringMap::iterator k = g->second.find(temp[1]);
				if (k != g->second.end())
				{
					if (temp.size() == 3)
					{
						unsigned long pos = tools::string::stringToUL(temp[2]);
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

		dodoStringMap::iterator k = global.find(temp[0]);
		if (k != global.end())
		{
			unsigned long pos = tools::string::stringToUL(temp[1]);
			if (pos >= 0 && pos <= k->second.size())
				return dodoString(1, k->second[pos]);
			else
				return __dodostring__;
		}

		dodoMap<dodoString, dodoStringMap>::iterator g = globalHash.find(temp[0]);
		if (g != globalHash.end())
		{
			k = g->second.find(temp[1]);
			if (k != g->second.end())
			{
				if (temp.size() == 3)
				{
					unsigned long pos = tools::string::stringToUL(temp[2]);
					if (pos >= 0 && pos <= k->second.size())
						return dodoString(1, k->second[pos]);
					else
						return __dodostring__;
				}
				else
					return k->second;
			}
		}

		dodoMap<dodoString, dodoStringArray>::iterator o = globalArray.find(temp[0]);
		if (o != globalArray.end())
		{
			unsigned long pos = tools::string::stringToUL(temp[1]);
			if (pos >= 0 && pos <= o->second.size())
			{
				if (temp.size() == 3)
				{
					unsigned long pos1 = tools::string::stringToUL(temp[2]);
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
			dodoMap<dodoString, dodoArray<dodoStringMap> >::iterator d = globalArrayHash.find(temp[0]);
			if (d != globalArrayHash.end())
			{
				unsigned long pos = tools::string::stringToUL(temp[1]);
				if (pos >= 0 && pos <= d->second.size())
				{
					k = d->second[pos].find(temp[2]);
					if (k != d->second[pos].end())
					{
						if (temp.size() == 4)
						{
							pos = tools::string::stringToUL(temp[3]);
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
processor::trim(const dodoString &statement)
{
	dodoString temp = tools::string::trim(statement, " \t\r\n", 4);

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
processor::display(const dodoString &path)
{
	CGI.print(this->process(path));
	CGI.flush();
}

//-------------------------------------------------------------------

