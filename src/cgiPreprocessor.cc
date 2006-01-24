/***************************************************************************
 *            cgiPreprocessor.cc
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

#include <cgiPreprocessor.h>

using namespace dodo;

cgiPreprocessor::cgiPreprocessor()
{
}

//-------------------------------------------------------------------

cgiPreprocessor::~cgiPreprocessor()
{
}

//-------------------------------------------------------------------

std::string 
cgiPreprocessor::read(const std::string &path,
					std::vector<unsigned long> &newLinePos)
{
	FILE *tpl = fopen(path.c_str(),"r");
	if (tpl == NULL)
		return __string__;
	
	struct stat st;

	if (::lstat(path.c_str(),&st) == -1)
		return __string__;
	
	char *stor = new char[st.st_size];
	
	fread(stor,st.st_size,1,tpl);
	fclose(tpl);
	
	std::string temp;
		
	temp.assign(stor,st.st_size);
	newLinePos.clear();
	
	register long i(0);
	for (;i<st.st_size;++i)
		if (temp[i] == '\n')
			newLinePos.push_back(i);
	newLinePos.push_back(i);
	
	delete [] stor;
	
	return temp;	
}

//-------------------------------------------------------------------

std::string 
cgiPreprocessor::preProcess(const std::string &path)
{	
	std::vector<unsigned long> newLinePos;
	
	std::string buffer = read(path,newLinePos);
	
	register unsigned long i(0), j(0), begin(0);
	
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
		{
			temp = buffer.substr(begin,i - begin);			
			if (temp.find(")>") != std::string::npos)
				#ifndef NO_EX
				{
					sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s Bracket `<(`",getLineNumber(newLinePos,j) + 1,path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
				}
				#else
					break;
				#endif			
				
			tpl.append(temp);
		}
		
		i += 2;

		if (buffer[i] == '>')
		{
			j = buffer.find("<)>",i);			
			if (j != std::string::npos)
			{
				i -= 2;
				j += 3;
				tpl.append(buffer.substr(i,j - i));
				continue;
			}
			else
				#ifndef NO_EX
				{
					sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s Bracket `<)>`",getLineNumber(newLinePos,j),path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
				}
				#else
					break;
				#endif
			
		}
		
		if (buffer[i] == '*')
		{	
			j = buffer.find("*)>",i);			
			if (j != std::string::npos)
			{
				j += 3;
				continue;
			}
			else
				#ifndef NO_EX
				{
					sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s Bracket `*)>`",getLineNumber(newLinePos,j),path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
				}
				#else
					break;
				#endif
		}
		
		j = buffer.find(")>",i);
		if (j == std::string::npos)
			#ifndef NO_EX
			{
				sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s Bracket `)>`",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
			}
			#else
				break;
			#endif
		
		if (j > 0 && buffer[j-1] == '*')
			#ifndef NO_EX
			{
				sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s Bracket `*)>",getLineNumber(newLinePos,j) + 1,path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
			}
			#else
				break;
			#endif
		
		if (j > 0 && buffer[j-1] == '<')
			#ifndef NO_EX
			{
				sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s Bracket `<)>",getLineNumber(newLinePos,j) + 1,path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
			}
			#else
				break;
			#endif
										
		temp = buffer.substr(i, j - i);
		
		if (temp.find("<(") != std::string::npos)
			#ifndef NO_EX
			{
				sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s Bracket `<(`",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
			}
			#else
				break;
			#endif
					
		j += 2;
		
		tpl.append(buffer.substr(i - 2,j - i + 2));
	}
	
	return tpl;
}

//-------------------------------------------------------------------

unsigned long 
cgiPreprocessor::getLineNumber(const std::vector<unsigned long> &newLinePos, 
								unsigned long pos)
{	
	o = newLinePos.begin();
	p = newLinePos.end();
	
	register unsigned long i(0);
	for (;o!=p;++o,++i)
		if (pos < *o)
			return i;
			
	return i;	
}

//-------------------------------------------------------------------
