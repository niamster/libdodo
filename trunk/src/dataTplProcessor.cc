/***************************************************************************
 *            dataTplProcessor.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/dataTplProcessor.h>

using namespace dodo::data::tpl;

const dodoString processor::statements[] =
{
	"dodo",
	"<(",
	")>",
	"<(>",
	"<)>",
	"<(*",
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
	"{",
	"}"
};

//-------------------------------------------------------------------

processor::processor() : continueFlag(false),
						 breakDeepness(0),
						 loopDeepness(0),
						 iterator(1),
						 namespaceDeepness(1)
{
	dodo[statements[PROCESSOR_STATEMENT_VERSION]] = PACKAGE_STRING;
	dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = "1";
}

//-------------------------------------------------------------------

processor::~processor()
{
}

//-------------------------------------------------------------------

dodoArray<unsigned long>
processor::detectNewLines(const dodoString &tpl)
{
	dodoArray<unsigned long> newLinePos;

	long i(0), j(tpl.size());
	for (; i < j; ++i)
	{
		if (tpl[i] == '\n')
		{
			newLinePos.push_back(i);
		}
	}
	newLinePos.push_back(i);

	return newLinePos;
}

//-------------------------------------------------------------------

dodoString
processor::preProcessString(const dodoString &buffer)
{
	return _preProcessString(buffer, "memory");
}

//-------------------------------------------------------------------

dodoString
processor::_preProcessString(const dodoString &buffer,
							 const dodoString &path)
{
	dodoArray<unsigned long> newLinePos = detectNewLines(buffer);

	unsigned long i(0), j(0), begin(0);

	while (true)
	{
		begin = j;

		i = buffer.find(statements[PROCESSOR_STATEMENT_OPEN_ST], begin);
		if (i == dodoString::npos)
		{
			j = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], begin);
			if (j != dodoString::npos)
			{
				throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `<(`", getLineNumber(newLinePos, j), path.c_str()));
			}

			break;
		}
		else
		{
			dodoString temp = dodoString(buffer.data() + begin, i - begin);

			j = temp.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], begin);
			if (j != dodoString::npos)
			{
				throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `)>`", getLineNumber(newLinePos, i), path.c_str()));
			}
		}

		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_NP], i);
			if (j != dodoString::npos)
			{
				j += 3;


				continue;
			}
			else
			{
				throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `<)>`", getLineNumber(newLinePos, j), path.c_str()));
			}
		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_COMM], i);
			if (j != dodoString::npos)
			{
				j += 3;

				continue;
			}
			else
			{
				throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `*)>`", getLineNumber(newLinePos, j), path.c_str()));
			}
		}

		j = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], i);
		if (j == dodoString::npos)
		{
			throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `)>`", getLineNumber(newLinePos, j), path.c_str()));
		}

		if (j > 0 && buffer[j - 1] == '*')
		{
			throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `*)>", getLineNumber(newLinePos, j), path.c_str()));
		}

		if (j > 0 && buffer[j - 1] == '<')
		{
			throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `<)>", getLineNumber(newLinePos, j), path.c_str()));
		}

		dodoString temp = dodoString(buffer.data() + i, j - i);

		if (temp.find(statements[PROCESSOR_STATEMENT_OPEN_ST]) != dodoString::npos)
		{
			throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__PREPROCESSSTRING, exception::ERRNO_LIBDODO, PROCESSOREX_NOTCLOSEDBRACKET, DATATPLPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s Bracket `<(`", getLineNumber(newLinePos, j), path.c_str()));
		}

		j += 2;
	}

	newLinePositions.push_back(newLinePos);

	return buffer;
}

//-------------------------------------------------------------------

dodoString
processor::preProcessFile(const dodoString &path)
{
	if (tplBasePath.empty())
	{
		return _preProcessString(tools::filesystem::getFileContents(path), path);
	}
	else
	{
		return _preProcessString(tools::filesystem::getFileContents(tplBasePath + FILE_DELIM + path), path);
	}
}

//-------------------------------------------------------------------

unsigned long
processor::getLineNumber(const dodoArray<unsigned long> &newLinePos,
						 unsigned long                  pos)
{
	dodoArray<unsigned long>::const_iterator o(newLinePos.begin()), p(newLinePos.end());

	unsigned long i(1);

	for (; o != p; ++o, ++i)
	{
		if (pos <= *o)
		{
			return i;
		}
	}

	return i - 1;
}

//-------------------------------------------------------------------

void
processor::processString(const dodoString &buffer,
						 io::channel      &tpl)
{
	_processString(preProcessString(buffer), "memory", tpl);

	newLinePositions.pop_back();
}

//-------------------------------------------------------------------

void
processor::processFile(const dodoString &path,
					   io::channel      &tpl)
{
	_processString(preProcessFile(path), path, tpl);

	newLinePositions.pop_back();
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

void
processor::_processString(const dodoString &buffer,
						  const dodoString &path,
						  io::channel      &tpl)
{
	unsigned long i(0), j(0), begin(0), k(0);
	unsigned long stI;

	dodoString temp;

	bool breakLoop = false;
	bool keywordNotFound = false;

	while (true)
	{
		begin = j;

		i = buffer.find(statements[PROCESSOR_STATEMENT_OPEN_ST], begin);
		if (i == dodoString::npos)
		{
			tpl.writeStream(dodoString(buffer.data() + begin, buffer.size() - begin));

			break;
		}
		else
		{
			tpl.writeStream(dodoString(buffer.data() + begin, i - begin));
		}

		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_NP], i);

			++i;
			tpl.writeStream(dodoString(buffer.data() + i, j - i));
			j += 3;

			continue;
		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_COMM], i);

			j += 3;

			continue;
		}

		j = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], i);

		for (stI = i; stI < j; ++stI)
		{
			if (buffer[stI] != '\t' && buffer[stI] != ' ' && buffer[stI] != '\r' && buffer[stI] != '\n')
			{
				break;
			}
		}

		temp = dodoString(buffer.data() + stI, j - stI);

		j += 2;

		keywordNotFound = false;

		switch (temp[0])
		{
		case 'p':

			k = temp.find(statements[PROCESSOR_STATEMENT_PRINT]);
			if (k == 0)
			{
				j = _print(j, dodoString(temp.data() + 5, temp.size() - 5), tpl, path);
			}
			else
			{
				keywordNotFound = true;
			}

			break;

		case 'i':

			k = temp.find(statements[PROCESSOR_STATEMENT_OPEN_IF]);
			if (k == 0)
			{
				++namespaceDeepness;

				j = _if(buffer, j, dodoString(temp.data() + 2, temp.size() - 2), tpl, path);

				cleanNamespace();

				--namespaceDeepness;
			}
			else
			{
				k = temp.find(statements[PROCESSOR_STATEMENT_INCLUDE]);
				if (k == 0)
				{
					j = _include(j, dodoString(temp.data() + 8, temp.size() - 8), tpl, path);
				}
				else
				{
					keywordNotFound = true;
				}
			}

			break;

		case 'f':

			k = temp.find(statements[PROCESSOR_STATEMENT_OPEN_FOR]);
			if (k == 0)
			{
				++loopDeepness;
				++namespaceDeepness;

				j = _for(buffer, j, dodoString(temp.data() + 3, temp.size() - 3), tpl, path);

				cleanNamespace();

				--namespaceDeepness;
				--loopDeepness;
			}
			else
			{
				keywordNotFound = true;
			}

			break;

		case 'b':

			k = temp.find(statements[PROCESSOR_STATEMENT_BREAK]);
			if (k == 0)
			{
				if (_break(j, dodoString(temp.data() + 5, temp.size() - 5), path))
				{
					breakLoop = true;
				}
			}
			else
			{
				keywordNotFound = true;
			}

			break;

		case 'c':

			k = temp.find(statements[PROCESSOR_STATEMENT_CONT]);
			if (k == 0)
			{
				if (loopDeepness > 0)
				{
					continueFlag = true;

					breakLoop = true;
				}
			}
			else
			{
				keywordNotFound = true;
			}

			break;

		case 'a':

			k = temp.find(statements[PROCESSOR_STATEMENT_ASSIGN]);
			if (k == 0)
			{
				j = _assign(j, dodoString(temp.data() + 6, temp.size() - 6), path);
			}
			else
			{
				keywordNotFound = true;
			}

			break;

		case 'n':

			k = temp.find(statements[PROCESSOR_STATEMENT_OPEN_NS]);
			if (k == 0)
			{
				++namespaceDeepness;

				j = _ns(buffer, j, tpl, path);

				cleanNamespace();

				--namespaceDeepness;
			}
			else
			{
				keywordNotFound = true;
			}

			break;

		default:

			keywordNotFound = true;
		}

		if (keywordNotFound)
		{
			tpl.writeStream(dodoString(buffer.data() + i - 2, j - i + 2));
		}

		if (breakLoop)
		{
			break;
		}

		if (breakDeepness > 0)
		{
			break;
		}

		if (continueFlag)
		{
			break;
		}
	}
}

//-------------------------------------------------------------------

void
processor::assign(dodoString                     varName,
				  const dodoArray<dodoStringMap> &varVal)
{
	if (tools::string::equal(varName, statements[PROCESSOR_STATEMENT_DODO]))
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX_ASSIGN, exception::ERRNO_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, DATATPLPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);
	}

	global.erase(varName);
	globalArray.erase(varName);
	globalHash.erase(varName);

	globalArrayHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodoString          varName,
				  const dodoStringMap &varVal)
{
	if (tools::string::equal(varName, statements[PROCESSOR_STATEMENT_DODO]))
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX_ASSIGN, exception::ERRNO_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, DATATPLPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);
	}

	global.erase(varName);
	globalArray.erase(varName);
	globalArrayHash.erase(varName);

	globalHash[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodoString            varName,
				  const dodoStringArray &varVal)
{
	if (tools::string::equal(varName, statements[PROCESSOR_STATEMENT_DODO]))
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX_ASSIGN, exception::ERRNO_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, DATATPLPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);
	}

	global.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);

	globalArray[varName] = varVal;
}

//-------------------------------------------------------------------

void
processor::assign(dodoString       varName,
				  const dodoString &varVal)
{
	if (tools::string::equal(varName, statements[PROCESSOR_STATEMENT_DODO]))
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX_ASSIGN, exception::ERRNO_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, DATATPLPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__);
	}

	globalArray.erase(varName);
	globalHash.erase(varName);
	globalArrayHash.erase(varName);

	global[varName] = varVal;
}

//-------------------------------------------------------------------

unsigned long
processor::_if(const dodoString &buffer,
			   unsigned long    start,
			   const dodoString &statement,
			   io::channel      &tpl,
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

	for (; i < j; ++i)
	{
		if ((j - i) > 1)
		{
			if (statement[i] == '=' && statement[i + 1] == '=')
			{
				temp2.push_back(dodoString(statement.data(), i));
				temp2.push_back(dodoString(statement.data() + i + 2, j - i - 2));

				break;
			}
			if (statement[i] == '!' && statement[i + 1] == '=')
			{
				invert = true;

				temp2.push_back(dodoString(statement.data(), i));
				temp2.push_back(dodoString(statement.data() + i + 2, j - i - 2));

				break;
			}
			if (statement[i] == '>' && statement[i + 1] == '=')
			{
				oper = OPERTYPE_GE;
				_float = true;

				temp2.push_back(dodoString(statement.data(), i));
				temp2.push_back(dodoString(statement.data() + i + 2, j - i - 2));

				break;
			}
			if (statement[i] == '<' && statement[i + 1] == '=')
			{
				oper = OPERTYPE_LE;
				_float = true;

				temp2.push_back(dodoString(statement.data(), i));
				temp2.push_back(dodoString(statement.data() + i + 2, j - i - 2));

				break;
			}
		}
		if (statement[i] == '<')
		{
			oper = OPERTYPE_LT;
			_float = true;

			temp2.push_back(dodoString(statement.data(), i));
			temp2.push_back(dodoString(statement.data() + i + 1, j - i - 1));

			break;
		}
		if (statement[i] == '>')
		{
			oper = OPERTYPE_GT;
			_float = true;

			temp2.push_back(dodoString(statement.data(), i));
			temp2.push_back(dodoString(statement.data() + i + 1, j - i - 1));

			break;
		}
		if (statement[i] == '!')
		{
			invert = true;

			temp1 = dodoString(statement.data() + i + 1, j - i - 1);

			break;
		}
	}

	bool accept(invert);

	if (temp2.size() != 2)
	{
		if (!tools::string::equal(getVar(temp1, start, path), statements[PROCESSOR_STATEMENT_FALSE]))
		{
			accept = !invert;
		}
		else
		{
			accept = invert;
		}
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
			{
				accept = !invert;
			}
		}
	}

	unsigned long u(blockEnd(buffer, start, statements[PROCESSOR_STATEMENT_OPEN_IF], statements[PROCESSOR_STATEMENT_CLOSE_IF], path)), v(0);
	bool found(true);

	try
	{
		v = blockEnd(buffer, start, statements[PROCESSOR_STATEMENT_OPEN_IF], statements[PROCESSOR_STATEMENT_ELSE], path);
	}
	catch (...)
	{
		found = false;
	}

	if (accept)
	{
		if (!found)
		{
			v = u;
		}

		_processString(dodoString(buffer.data() + start, v - start), path, tpl);
	}
	else
	{
		if (found)
		{
			v = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], v) + 2;

			_processString(dodoString(buffer.data() + v, u - v), path, tpl);
		}
	}

	return buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
processor::blockEnd(const dodoString &buffer,
					unsigned long    start,
					const dodoString &st,
					const dodoString &ts,
					const dodoString &path)
{
	unsigned long u, m(start), _st(1), b, p, stLen(st.size()), tsLen(ts.size());

	while (true)
	{
		u = buffer.find(statements[PROCESSOR_STATEMENT_OPEN_ST], m);
		if (u == dodoString::npos)
		{
			throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX_BLOCKEND, exception::ERRNO_LIBDODO, PROCESSOREX_WRONGBLOCK, DATATPLPROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));
		}

		b = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], u + 2);
		if (b == dodoString::npos)
		{
			throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX_BLOCKEND, exception::ERRNO_LIBDODO, PROCESSOREX_WRONGBLOCK, DATATPLPROCESSOREX_WRONGBLOCK_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));
		}

		for (p = u; p < b; ++p)
		{
			if (buffer[p] != ' ' && buffer[p] != '\t' && buffer[p] != '\n' && buffer[p] != '\r')
			{
				if (tools::string::equal(dodoString(buffer.data() + p, tsLen), ts))
				{
					--_st;
				}
				else
				{
					if (tools::string::equal(dodoString(buffer.data() + p, stLen), st))
					{
						++_st;
					}
				}
			}
		}

		if (_st == 0)
		{
			break;
		}

		m = b + 2;
	}

	return u;
}

//-------------------------------------------------------------------

unsigned long
processor::_include(unsigned long    start,
					const dodoString &statement,
					io::channel      &tpl,
					const dodoString &path)
{
	dodoString temp1 = getVar(statement, start, path);

	if (!tools::string::equal(temp1, path) && !tools::misc::isInList(processed, temp1, false))
	{
		if (!tplBasePath.empty())
		{
			temp1 = tplBasePath + FILE_DELIM + temp1;
		}

		if (tools::filesystem::exists(temp1))
		{
			processed.push_back(path);

			processFile(temp1, tpl);

			processed.pop_back();
		}
	}

	return start;
}

//-------------------------------------------------------------------

unsigned long
processor::_print(unsigned long    start,
				  const dodoString &statement,
				  io::channel      &tpl,
				  const dodoString &path)
{
	dodoStringArray temp = tools::misc::split(statement, statements[PROCESSOR_STATEMENT_COMA]);
	if (temp.size() <= 1)
	{
		tpl.writeStream(getVar(statement, start, path));
	}
	else
	{
		dodoStringArray::iterator i(temp.begin()), j(temp.end());
		for (; i != j; ++i)
		{
			tpl.writeStream(getVar(*i, start, path));
		}
	}

	return start;
}

//-------------------------------------------------------------------

bool
processor::_break(unsigned long    start,
				  const dodoString &statement,
				  const dodoString &path)
{
	if (loopDeepness > 0)
	{
		breakDeepness = tools::string::stringToUL(getVar(statement, start, path));

		if (breakDeepness == 0)
		{
			breakDeepness = 1;
		}
		else if (breakDeepness > loopDeepness)
		{
			breakDeepness = loopDeepness;
		}

		return true;
	}

	return false;
}

//-------------------------------------------------------------------

unsigned long
processor::_assign(unsigned long    start,
				   const dodoString &statement,
				   const dodoString &path)
{
	dodoStringArray temp = tools::misc::split(statement, statements[PROCESSOR_STATEMENT_ASSIGN_OP], 2);

	if (temp.size() == 0)
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__ASSIGN, exception::ERRNO_LIBDODO, PROCESSOREX_WRONGASSIGNSTATEMENT, DATATPLPROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));
	}

	dodoString varName = trim(temp[0]);
	if (varName.size() == 0)
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__ASSIGN, exception::ERRNO_LIBDODO, PROCESSOREX_WRONGASSIGNSTATEMENT, DATATPLPROCESSOREX_WRONGASSIGNSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));
	}

	if (varName[0] == '$')
	{
		varName = dodoString(varName.data() + 1, varName.size() - 1);
	}

	if (tools::string::equal(varName, statements[PROCESSOR_STATEMENT_DODO]))
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__ASSIGN, exception::ERRNO_LIBDODO, PROCESSOREX_DODOISRESERVEDVARNAME, DATATPLPROCESSOREX_DODOISRESERVEDVARNAME_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));
	}

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
			   unsigned long    start,
			   io::channel      &tpl,
			   const dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[PROCESSOR_STATEMENT_OPEN_NS], statements[PROCESSOR_STATEMENT_CLOSE_NS], path));

	_processString(dodoString(buffer.data() + start, u - start), path, tpl);

	return buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], u) + 2;
}

//-------------------------------------------------------------------

unsigned long
processor::_for(const dodoString &buffer,
				unsigned long    start,
				const dodoString &statement,
				io::channel      &tpl,
				const dodoString &path)
{
	unsigned long u(blockEnd(buffer, start, statements[PROCESSOR_STATEMENT_OPEN_FOR], statements[PROCESSOR_STATEMENT_CLOSE_FOR], path));

	unsigned long p = statement.find(statements[PROCESSOR_STATEMENT_DOLLAR]);
	unsigned long i(p), j(statement.size());

	for (; i < j; ++i)
	{
		if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
		{
			break;
		}
	}
	--i;

	dodoString varName = dodoString(statement.data() + p + 1, i - p);

	bool key(false);
	dodoString keyName;

	p = statement.find(statements[PROCESSOR_STATEMENT_KEY_VALUE], i + 1);
	if (p != dodoString::npos)
	{
		key = true;
		p = statement.find(statements[PROCESSOR_STATEMENT_DOLLAR], p + 2);

		for (i = p; i < j; ++i)
		{
			if (statement[i] == ' ' || statement[i] == '\t' || statement[i] == '\n')
			{
				break;
			}
		}
		--i;

		keyName = varName;
		varName = dodoString(statement.data() + p + 1, i - p);
	}

	p = statement.find(statements[PROCESSOR_STATEMENT_IN], i + 1);
	if (p == dodoString::npos)
	{
		throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX__FOR, exception::ERRNO_LIBDODO, PROCESSOREX_WRONGFORSTATEMENT, DATATPLPROCESSOREX_WRONGFORSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));
	}

	dodoString targetVar = getVarName(dodoString(statement.data() + p + 2), start, path);
	dodoString forSpace = dodoString(buffer.data() + start, u - start);

	u = buffer.find(statements[PROCESSOR_STATEMENT_CLOSE_ST], u) + 2;

	if (targetVar[0] == '$')
	{
		targetVar = dodoString(targetVar.data() + 1, targetVar.size() - 1);

		dodoStringArray temp = tools::misc::split(targetVar, statements[PROCESSOR_STATEMENT_DOT]);

		if (temp.size() == 1)
		{
			dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsi = localHash.rbegin();
			dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsj = localHash.rend();
			for (; lhpnsi != lhpnsj; ++lhpnsi)
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
						dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

						if (key)
						{
							lns[keyName] = k->first;
						}
						lns[varName] = k->second;

						_processString(forSpace, path, tpl);

						if (breakDeepness > 0)
						{
							--breakDeepness;

							break;
						}
						if (continueFlag)
						{
							continueFlag = false;
						}
					}

					iterator =  iteratorPrev;
					dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

					return u;
				}
			}

			dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsi = local.rbegin();
			dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsj = local.rend();
			for (; lpnsi != lpnsj; ++lpnsi)
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
						dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

						if (key)
						{
							lns[keyName] = tools::string::lToString(i);
						}
						lns[varName] = dodoString(1, k->second[i]);

						_processString(forSpace, path, tpl);

						if (breakDeepness > 0)
						{
							--breakDeepness;

							break;
						}
						if (continueFlag)
						{
							continueFlag = false;
						}
					}

					iterator =  iteratorPrev;
					dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
					dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
					{
						lns[keyName] = tools::string::lToString(i);
					}
					lns[varName] = dodoString(1, k->second[i]);

					_processString(forSpace, path, tpl);

					if (breakDeepness > 0)
					{
						--breakDeepness;

						break;
					}
					if (continueFlag)
					{
						continueFlag = false;
					}
				}

				iterator =  iteratorPrev;
				dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
					dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
					{
						lns[keyName] = k->first;
					}
					lns[varName] = k->second;

					_processString(forSpace, path, tpl);

					if (breakDeepness > 0)
					{
						--breakDeepness;

						break;
					}
					if (continueFlag)
					{
						continueFlag = false;
					}
				}

				iterator =  iteratorPrev;
				dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
					dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
					{
						lns[keyName] = tools::string::lToString(keyNIter);
					}
					lns[varName] = *k;

					_processString(forSpace, path, tpl);

					if (breakDeepness > 0)
					{
						--breakDeepness;

						break;
					}
					if (continueFlag)
					{
						continueFlag = false;
					}
				}

				iterator =  iteratorPrev;
				dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
					dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

					if (key)
					{
						lns[keyName] = tools::string::lToString(keyNIter);
					}
					lnsh[varName] = *k;

					_processString(forSpace, path, tpl);

					if (breakDeepness > 0)
					{
						--breakDeepness;

						break;
					}
					if (continueFlag)
					{
						continueFlag = false;
					}
				}

				iterator =  iteratorPrev;
				dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

				return u;
			}
		}
		else
		{
			if (temp.size() == 2)
			{
				dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsi = localHash.rbegin();
				dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsj = localHash.rend();
				for (; lhpnsi != lhpnsj; ++lhpnsi)
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
								dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

								if (key)
								{
									lns[keyName] = tools::string::lToString(i);
								}
								lns[varName] = dodoString(1, k->second[i]);

								_processString(forSpace, path, tpl);

								if (breakDeepness > 0)
								{
									--breakDeepness;

									break;
								}
								if (continueFlag)
								{
									continueFlag = false;
								}
							}

							iterator =  iteratorPrev;
							dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
							dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

							if (key)
							{
								lns[keyName] = tools::string::lToString(i);
							}
							lns[varName] = dodoString(1, k->second[i]);

							_processString(forSpace, path, tpl);

							if (breakDeepness > 0)
							{
								--breakDeepness;

								break;
							}
							if (continueFlag)
							{
								continueFlag = false;
							}
						}

						iterator =  iteratorPrev;
						dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
							dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

							if (key)
							{
								lns[keyName] = tools::string::lToString(i);
							}
							lns[varName] = dodoString(1, o->second[pos][i]);

							_processString(forSpace, path, tpl);

							if (breakDeepness > 0)
							{
								--breakDeepness;

								break;
							}
							if (continueFlag)
							{
								continueFlag = false;
							}
						}

						iterator =  iteratorPrev;
						dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
							dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

							if (key)
							{
								lns[keyName] = k->first;
							}
							lns[varName] = k->second;

							_processString(forSpace, path, tpl);

							if (breakDeepness > 0)
							{
								--breakDeepness;

								break;
							}
							if (continueFlag)
							{
								continueFlag = false;
							}
						}

						iterator =  iteratorPrev;
						dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
									dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

									if (key)
									{
										lns[keyName] = tools::string::lToString(i);
									}
									lns[varName] = dodoString(1, k->second[i]);

									_processString(forSpace, path, tpl);

									if (breakDeepness > 0)
									{
										--breakDeepness;

										break;
									}
									if (continueFlag)
									{
										continueFlag = false;
									}
								}

								iterator =  iteratorPrev;
								dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

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
			dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iterator);

			if (key)
			{
				lns[keyName] = tools::string::lToString(i);
			}
			lns[varName] = dodoString(1, targetVar[i]);

			_processString(forSpace, path, tpl);

			if (breakDeepness > 0)
			{
				--breakDeepness;

				break;
			}
			if (continueFlag)
			{
				continueFlag = false;
			}
		}

		iterator =  iteratorPrev;
		dodo[statements[PROCESSOR_STATEMENT_ITERATOR]] = tools::string::lToString(iteratorPrev);

		return u;
	}

	return u;
}

//-------------------------------------------------------------------

dodoString
processor::getVarName(const dodoString &a_varName,
					  unsigned long    start,
					  const dodoString &path)
{
	dodoString varName = trim(a_varName), tempVar;

	unsigned long u, b, m(0), ob, cb, i, c;

	while (true)
	{
		ob = 1;
		cb = 0;

		u = varName.find(statements[PROCESSOR_STATEMENT_OPEN_VARPART], m);
		if (u == dodoString::npos)
		{
			break;
		}

		c = u;
		while (true)
		{
			b = varName.find(statements[PROCESSOR_STATEMENT_CLOSE_VARPART], c + 1);
			if (b == dodoString::npos)
			{
				throw exception::basic(exception::ERRMODULE_DATATPLPROCESSOR, PROCESSOREX_GETVARNAME, exception::ERRNO_LIBDODO, PROCESSOREX_WRONGVARSTATEMENT, DATATPLPROCESSOREX_WRONGVARSTATEMENT_STR, __LINE__, __FILE__, tools::string::format(" Line: %li File: %s", getLineNumber(newLinePositions.back(), start), path.c_str()));
			}

			++cb;

			for (i = c + 1; i < b; ++i)
			{
				if (varName[i] == '{')
				{
					++ob;
				}
			}

			c = b;

			if (cb == ob)
			{
				break;
			}
		}

		tempVar = trim(dodoString(varName.data() + u + 1, b - 1 - u));
		if (tempVar[0] == '$')
		{
			tempVar = getVar(tempVar, start, path);
		}

		varName.replace(u, b + 1 - u, tempVar, 0, tempVar.size());

		m = b - 1;
	}

	return varName;
}

//-------------------------------------------------------------------

dodoString
processor::getVar(const dodoString &a_varName,
				  unsigned long    start,
				  const dodoString &path)
{
	dodoString varName = getVarName(a_varName, start, path);

	if (varName[0] != '$')
	{
		return varName;
	}
	else
	{
		varName.erase(0, 1);
	}

	dodoStringArray temp = tools::misc::split(varName, statements[PROCESSOR_STATEMENT_DOT]);

	if (temp.size() == 1)
	{
		if (tools::string::equal(varName, statements[PROCESSOR_STATEMENT_DODO]))
		{
			return "cgi framework libdodo";
		}

		dodoStringMap::iterator k;

		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsi = local.rbegin();
		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsj = local.rend();
		for (; lpnsi != lpnsj; ++lpnsi)
		{
			k = lpnsi->second.find(varName);
			if (k != lpnsi->second.end())
			{
				return k->second;
			}
		}

		k = global.find(varName);
		if (k != global.end())
		{
			return k->second;
		}

		return __dodostring__;
	}
	else
	{
		if (tools::string::equal(temp[0], statements[PROCESSOR_STATEMENT_DODO]))
		{
			dodoStringMap::iterator k = dodo.find(temp[1]);
			if (k != dodo.end())
			{
				if (temp.size() == 3)
				{
					unsigned long pos = tools::string::stringToUL(temp[2]);
					if (pos >= 0 && pos <= k->second.size())
					{
						return dodoString(1, k->second[pos]);
					}
					else
					{
						return __dodostring__;
					}
				}
				else
				{
					return k->second;
				}
			}

			return __dodostring__;
		}

		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsi = local.rbegin();
		dodoMap<unsigned int, dodoStringMap>::reverse_iterator lpnsj = local.rend();
		for (; lpnsi != lpnsj; ++lpnsi)
		{
			dodoStringMap::iterator k = lpnsi->second.find(temp[0]);
			if (k != lpnsi->second.end())
			{
				unsigned long pos = tools::string::stringToUL(temp[1]);
				if (pos >= 0 && pos <= k->second.size())
				{
					return dodoString(1, k->second[pos]);
				}
				else
				{
					return __dodostring__;
				}
			}
		}

		dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsi = localHash.rbegin();
		dodoMap<unsigned int, dodoMap<dodoString, dodoStringMap> >::reverse_iterator lhpnsj = localHash.rend();
		for (; lhpnsi != lhpnsj; ++lhpnsi)
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
						{
							return dodoString(1, k->second[pos]);
						}
						else
						{
							return __dodostring__;
						}
					}
					else
					{
						return k->second;
					}
				}
			}
		}

		dodoStringMap::iterator k = global.find(temp[0]);
		if (k != global.end())
		{
			unsigned long pos = tools::string::stringToUL(temp[1]);
			if (pos >= 0 && pos <= k->second.size())
			{
				return dodoString(1, k->second[pos]);
			}
			else
			{
				return __dodostring__;
			}
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
					{
						return dodoString(1, k->second[pos]);
					}
					else
					{
						return __dodostring__;
					}
				}
				else
				{
					return k->second;
				}
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
					{
						return dodoString(1, o->second[pos][pos1]);
					}
					else
					{
						return __dodostring__;
					}
				}
				else
				{
					return o->second[pos];
				}
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
							{
								return dodoString(1, k->second[pos]);
							}
							else
							{
								return __dodostring__;
							}
						}
						else
						{
							return k->second;
						}
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
		temp = dodoString(temp.data() + 1, temp.size() - 2);
	}
	else
	{
		if (temp[0] == '\'' && temp[i] == '\'')
		{
			temp = dodoString(temp.data() + 1, temp.size() - 2);
		}
		else
		{
			if (temp[0] == '`' && temp[i] == '`')
			{
				temp = dodoString(temp.data() + 1, temp.size() - 2);
			}
		}
	}

	return temp;
}

//-------------------------------------------------------------------

