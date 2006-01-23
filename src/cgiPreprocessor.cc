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
cgiPreprocessor::preProcess(const std::string &path) const
{
	std::vector<unsigned long> newLine;
	
	return process(read(path,newLine), path, newLine);
}

//-------------------------------------------------------------------

std::string 
cgiPreprocessor::read(const std::string &path,
					std::vector<unsigned long> &newLinePos) const
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
cgiPreprocessor::process(const std::string &buffer, 
						std::string path,
						const std::vector<unsigned long> &newLinePos) const
{
	std::vector<unsigned long> newLine;
	
	register unsigned long i(0), j(0), begin(0), k(0);
	
	std::string tpl, temp2;
	stringArr tempArr;
	
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
		
/*		if (buffer[i] == '>')
		{
			j = buffer.find("<)>",i);
			
			if (j != std::string::npos)
			{
				i += 1;
				tpl.append(buffer.substr(i,j - 1 - i));
				j += 3;
				continue;
			}
		}*/
		
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
					sprintf(message,CGIPREPROCESSOR_NOTCLOSEDCOMMENTBRACKET_STR " Line: %li File: %s",getLineNumber(newLinePos,j),path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDCOMMENTBRACKET,message,__LINE__,__FILE__);
				}
				#else
					break;
				#endif
		}
		
		j = buffer.find(")>",i);
		if (j == std::string::npos)
			#ifndef NO_EX
			{
				sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
			}
			#else
				break;
			#endif
		
		if (j > 0 && buffer[j-1] == '*')
			#ifndef NO_EX
			{
				sprintf(message,CGIPREPROCESSOR_NOTCLOSEDCOMMENTBRACKET_STR " Line: %li File: %s",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDCOMMENTBRACKET,message,__LINE__,__FILE__);
			}
			#else
				break;
			#endif
							
		temp = buffer.substr(i, j - i);
		
		if (temp.find("<(") != std::string::npos)
			#ifndef NO_EX
			{
				sprintf(message,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR " Line: %li File: %s",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,message,__LINE__,__FILE__);
			}
			#else
				break;
			#endif
					
		j += 2;
		
		k = temp.find("include");
		
		if (k != std::string::npos)
		{
			temp1 = tools::trim(temp.substr(k + 8)," \t\n\"'",5);

			if (temp1[0] == '$')
				temp1 = global[temp1.substr(1)];
			
			if (strcmp(temp1.c_str(),path.c_str()) != 0 && !recursive(temp1))
			{
				processed.push_back(path);
				tpl.append(process(read(temp1, newLine), temp1, newLine));
				processed.pop_back();
			}
		}				
		else
			tpl.append(buffer.substr(i - 2,j - i + 2));
	}
	
	return tpl;
}

//-------------------------------------------------------------------

bool 
cgiPreprocessor::recursive(const std::string &path) const
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
cgiPreprocessor::assign(const std::string &varName, 
						const std::string &varVal)
{
	global.realArr[varName] = varVal;
}

//-------------------------------------------------------------------

unsigned long 
cgiPreprocessor::getLineNumber(const std::vector<unsigned long> &newLinePos, 
								unsigned long pos) const
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
