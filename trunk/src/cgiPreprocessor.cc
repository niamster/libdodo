/***************************************************************************
 *            cgiPreprocessor.cc
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

#include <libdodo/cgiPreprocessor.h>

using namespace dodo;

const dodoString cgiPreprocessor::statements[] = { "dodo",
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
												   "==",
												   "!=",
												   "<=",
												   ">=",
												   "<",
												   ">",
												   "{",
												   "}" };

//-------------------------------------------------------------------

cgiPreprocessor::cgiPreprocessor()
{
}

//-------------------------------------------------------------------

cgiPreprocessor::~cgiPreprocessor()
{
}

//-------------------------------------------------------------------

dodoArray<unsigned long>
cgiPreprocessor::detectNewLines(const dodoString &tpl)
{
	dodoArray<unsigned long> newLinePos;

	long i(0), j(tpl.size());
	for (; i < j; ++i)
		if (tpl[i] == '\n')
			newLinePos.push_back(i);
	newLinePos.push_back(i);

	return newLinePos;
}

//-------------------------------------------------------------------

dodoString
cgiPreprocessor::preProcessString(const dodoString &buffer)
{
	_preProcessString(buffer, "memory");
}

//-------------------------------------------------------------------

dodoString
cgiPreprocessor::_preProcessString(const dodoString &buffer,
								   const dodoString &path)
{
	dodoArray<unsigned long> newLinePos = detectNewLines(buffer);

	unsigned long i(0), j(0), begin(0);

	while (true)
	{
		begin = j;

		i = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST], begin);
		if (i == dodoString::npos)
		{
			j = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], begin);
			if (j != dodoString::npos)
				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `<(`", getLineNumber(newLinePos, j), path.c_str()));

			break;
		}
		else
		{
			dodoString temp = buffer.substr(begin, i - begin);

			j = temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], begin);
			if (j != dodoString::npos)
				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `)>`", getLineNumber(newLinePos, i), path.c_str()));
		}

		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NP], i);
			if (j != dodoString::npos)
			{
				j += 3;


				continue;
			}
			else
				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `<)>`", getLineNumber(newLinePos, j), path.c_str()));

		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_COMM], i);
			if (j != dodoString::npos)
			{
				j += 3;

				continue;
			}
			else
				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `*)>`", getLineNumber(newLinePos, j), path.c_str()));
		}

		j = buffer.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST], i);
		if (j == dodoString::npos)
			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `)>`", getLineNumber(newLinePos, j), path.c_str()));

		if (j > 0 && buffer[j - 1] == '*')
			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `*)>", getLineNumber(newLinePos, j), path.c_str()));

		if (j > 0 && buffer[j - 1] == '<')
			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `<)>", getLineNumber(newLinePos, j), path.c_str()));

		dodoString temp = buffer.substr(i, j - i);

		if (temp.find(statements[CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST]) != dodoString::npos)
			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX__PREPROCESSSTRING, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, toolsString::format(" Line: %li File: %s Bracket `<(`", getLineNumber(newLinePos, j), path.c_str()));

		j += 2;
	}

	newLinePositions.push_back(newLinePos);

	return buffer;
}

//-------------------------------------------------------------------

dodoString
cgiPreprocessor::preProcess(const dodoString &path)
{
	if (tplBasePath.empty())
		return _preProcessString(toolsFilesystem::getFileContents(path), path);
	else
		return _preProcessString(toolsFilesystem::getFileContents(tplBasePath + FILE_DELIM + path), path);
}

//-------------------------------------------------------------------

unsigned long
cgiPreprocessor::getLineNumber(const dodoArray<unsigned long> &newLinePos,
							   unsigned long pos)
{
	dodoArray<unsigned long>::const_iterator o(newLinePos.begin()), p(newLinePos.end());

	unsigned long i(1);

	for (; o != p; ++o, ++i)
		if (pos <= *o)
			return i;

	return i - 1;
}

//-------------------------------------------------------------------
