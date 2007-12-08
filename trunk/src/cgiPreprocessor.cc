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

dodoString
cgiPreprocessor::read(const dodoString &path,
					  dodoArray<unsigned long> &newLinePos)
{
	FILE *tpl = fopen(path.c_str(), "r");
	if (tpl == NULL)
		throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	struct stat st;

	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_READ, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	char *stor = new char[st.st_size];

	fread(stor, st.st_size, 1, tpl);
	fclose(tpl);

	dodoString temp;

	temp.assign(stor, st.st_size);
	newLinePos.clear();

	long i(0);
	for (; i < st.st_size; ++i)
		if (temp[i] == '\n')
			newLinePos.push_back(i);
	newLinePos.push_back(i);

	delete [] stor;

	return temp;
}

//-------------------------------------------------------------------

dodoString
cgiPreprocessor::preProcess(const dodoString &path)
{
	dodoArray<unsigned long> newLinePos;

	dodoString buffer = read(path, newLinePos);

	unsigned long i(0), j(0), begin(0);

	while (true)
	{
		begin = j;

		i = buffer.find(statements[OPEN_ST], begin);
		if (i == dodoString::npos)
		{
			j = buffer.find(statements[CLOSE_ST], begin);
			if (j != dodoString::npos)
			{
				char message[128];
				sprintf(message, " Line: %li File: %s Bracket `<(`", getLineNumber(newLinePos, j), path.c_str());

				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
			}

			break;
		}
		else
		{
			dodoString temp = buffer.substr(begin, i - begin);

			j = temp.find(statements[CLOSE_ST], begin);
			if (j != dodoString::npos)
			{
				char message[128];
				sprintf(message, " Line: %li File: %s Bracket `)>`", getLineNumber(newLinePos, i), path.c_str());

				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
			}
		}

		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find(statements[CLOSE_NP], i);
			if (j != dodoString::npos)
			{
				j += 3;


				continue;
			}
			else
			{
				char message[128];
				sprintf(message, " Line: %li File: %s Bracket `<)>`", getLineNumber(newLinePos, j), path.c_str());

				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
			}

		}

		if (buffer[i] == '*')
		{
			j = buffer.find(statements[CLOSE_COMM], i);
			if (j != dodoString::npos)
			{
				j += 3;

				continue;
			}
			else
			{
				char message[128];
				sprintf(message, " Line: %li File: %s Bracket `*)>`", getLineNumber(newLinePos, j), path.c_str());

				throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
			}
		}

		j = buffer.find(statements[CLOSE_ST], i);
		if (j == dodoString::npos)
		{
			char message[128];
			sprintf(message, " Line: %li File: %s Bracket `)>`", getLineNumber(newLinePos, j), path.c_str());

			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
		}

		if (j > 0 && buffer[j - 1] == '*')
		{
			char message[128];
			sprintf(message, " Line: %li File: %s Bracket `*)>", getLineNumber(newLinePos, j), path.c_str());

			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
		}

		if (j > 0 && buffer[j - 1] == '<')
		{
			char message[128];
			sprintf(message, " Line: %li File: %s Bracket `<)>", getLineNumber(newLinePos, j), path.c_str());

			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
		}

		dodoString temp = buffer.substr(i, j - i);

		if (temp.find(statements[OPEN_ST]) != dodoString::npos)
		{
			char message[128];
			sprintf(message, " Line: %li File: %s Bracket `<(`", getLineNumber(newLinePos, j), path.c_str());

			throw baseEx(ERRMODULE_CGIPREPROCESSOR, CGIPREPROCESSOREX_PREPROCESS, ERR_LIBDODO, CGIPREPROCESSOREX_NOTCLOSEDBRACKET, CGIPREPROCESSOREX_NOTCLOSEDBRACKET_STR, __LINE__, __FILE__, message);
		}

		j += 2;
	}

	newLinePositions.push_back(newLinePos);

	return buffer;
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
