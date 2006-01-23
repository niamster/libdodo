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
	return process(read(path), path);
}

//-------------------------------------------------------------------

std::string 
cgiPreprocessor::read(const std::string &path) const
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
	
	delete [] stor;
	
	return temp;	
}

//-------------------------------------------------------------------

std::string 
cgiPreprocessor::process(const std::string &buffer, 
						std::string path) const
{
	register unsigned long i(0), j(0), begin(0), k(0);
	
	std::string tpl, temp2;
	stringArr tempArr;
	
	while (true)
	{	
		begin = j;
		
		i = buffer.find("<(",i);
		if (i == std::string::npos)
		{
			tpl.append(buffer.substr(begin));
			break;
		}
		else
			tpl.append(buffer.substr(begin,i - begin));
		
		i += 2;
		
		j = buffer.find(")>",i);
		if (j == std::string::npos)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_CGIPREPROCESSOR,CGIPREPROCESSOR_PROCESS,ERR_LIBDODO,CGIPREPROCESSOR_NOTCLOSEDBRACKET,CGIPREPROCESSOR_NOTCLOSEDBRACKET_STR,__LINE__,__FILE__);
			#else
				break;
			#endif
		
		if (buffer[i] == '*' && buffer[j-1] == '*')
		{
			j += 2;
			continue;
		}
			
		temp = buffer.substr(i, j - i);
		j += 2;
				
		k = temp.find("include");
		
		if (k != std::string::npos)
		{
			temp1 = tools::trim(temp.substr(k + 8)," \t\n",3);

			if (temp1[0] != '$')
				if (strcmp(temp1.c_str(),path.c_str()) != 0 && !recursive(temp1))
				{
					processed.push_back(path);
					tpl.append(process(read(temp1), temp1));
					processed.pop_back();
				}
		}
		else
			tpl.append(buffer.substr(i - 2,j - i + 2));
		/*else
		{
			k = temp.find("define");
			if (k != std::string::npos)
			{
				temp1 = tools::lTrim(temp.substr(k + 7)," \t",2);
				k = temp1.find(" ");
				if (k == std::string::npos)
					k = temp1.find("\t");
					
				defNames.push_back(temp1.substr(0,k));
				defNames.push_back(tools::lTrim(temp1.substr(k)," \t",2));
			}
		}*/
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
