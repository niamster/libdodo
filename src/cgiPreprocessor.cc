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

dodoString 
cgiPreprocessor::read(const dodoString &path,
					dodoArray<unsigned long> &newLinePos)
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
	
	dodoString temp;
		
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

dodoString 
cgiPreprocessor::preProcess(const dodoString &path)
{	
	dodoArray<unsigned long> newLinePos;
	
	dodoString buffer = read(path,newLinePos);
	
	register unsigned long i(0), j(0), begin(0);
	
	dodoString tpl;
	
	while (true)
	{	
		begin = j;
		
		i = buffer.find("<(",begin);
		if (i == dodoString::npos)
		{
			j = buffer.find(")>",begin);
			if (j != dodoString::npos)
				#ifndef NO_EX
				{
					char message[128];
					sprintf(message," Line: %li File: %s Bracket `<(`",getLineNumber(newLinePos,j),path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
				}
				#else
					break;
				#endif	
							
			tpl.append(buffer.substr(begin));
			break;
		}
		else
		{
			dodoString temp = buffer.substr(begin,i - begin);
			
			j = temp.find(")>",begin);
			if (j != dodoString::npos)
				#ifndef NO_EX
				{
					char message[128];
					sprintf(message," Line: %li File: %s Bracket `)>`",getLineNumber(newLinePos,i),path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
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
			if (j != dodoString::npos)
			{
				i -= 2;
				j += 3;
				tpl.append(buffer.substr(i,j - i));
				continue;
			}
			else
				#ifndef NO_EX
				{
					char message[128];
					sprintf(message," Line: %li File: %s Bracket `<)>`",getLineNumber(newLinePos,j),path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
				}
				#else
					break;
				#endif
			
		}
		
		if (buffer[i] == '*')
		{	
			j = buffer.find("*)>",i);			
			if (j != dodoString::npos)
			{
				j += 3;
				continue;
			}
			else
				#ifndef NO_EX
				{
					char message[128];
					sprintf(message," Line: %li File: %s Bracket `*)>`",getLineNumber(newLinePos,j),path.c_str());
					throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
				}
				#else
					break;
				#endif
		}
		
		j = buffer.find(")>",i);
		if (j == dodoString::npos)
			#ifndef NO_EX
			{
				char message[128];
				sprintf(message," Line: %li File: %s Bracket `)>`",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
			}
			#else
				break;
			#endif
		
		if (j > 0 && buffer[j-1] == '*')
			#ifndef NO_EX
			{
				char message[128];
				sprintf(message," Line: %li File: %s Bracket `*)>",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
			}
			#else
				break;
			#endif
		
		if (j > 0 && buffer[j-1] == '<')
			#ifndef NO_EX
			{
				char message[128];
				sprintf(message," Line: %li File: %s Bracket `<)>",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
			}
			#else
				break;
			#endif
										
		dodoString temp = buffer.substr(i, j - i);
		
		if (temp.find("<(") != dodoString::npos)
			#ifndef NO_EX
			{
				char message[128];
				sprintf(message," Line: %li File: %s Bracket `<(`",getLineNumber(newLinePos,j),path.c_str());
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__,message);
			}
			#else
				break;
			#endif
		
		j += 2;
		
		tpl.append(buffer.substr(i - 2,j - i + 2));
	}
	
	linePoss.push_back(newLinePos);
	
	return tpl;
}

//-------------------------------------------------------------------

unsigned long 
cgiPreprocessor::getLineNumber(const dodoArray<unsigned long> &newLinePos, 
								unsigned long pos)
{	
	dodoArray<unsigned long>::const_iterator o(newLinePos.begin()), p(newLinePos.end());
	
	register unsigned long i(1);
	
	for (;o!=p;++o,++i)
		if (pos <= *o)
			return i;
			
	return i - 1;	
}

//-------------------------------------------------------------------
