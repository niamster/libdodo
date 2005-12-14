/***************************************************************************
 *            tools.cc
 *
 *  Wed Aug 24 00:43:21 2005
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <tools.h>

using namespace dodo;

inline std::string
tools::dummy(const std::string &data)
{
	return data;
}

//-------------------------------------------------------------------

void 
tools::replace(pchar needle, 
		pchar replacement,
		std::string &data)
{
	register unsigned int i(0),j(strlen(needle)),k(strlen(replacement));
	while (true)
	{
		i = data.find(needle,i);
		if (i==std::string::npos)
			break;
		data.replace(i,j,replacement,0,k);
		i += k;
	}
}

//-------------------------------------------------------------------

tools::tools()
{
	#ifdef CODECONV_EXT
		convSet	= false;
	#endif
}

//-------------------------------------------------------------------

tools::~tools()
{
	#ifdef CODECONV_EXT	
		if(convSet)
			iconv_close(conv);
	#endif
}

//-------------------------------------------------------------------

stringArr 
tools::explode(const std::string &fields,
			escape escapeF,
			const std::string &separator, 
			int limit)
{
	register unsigned int i(0), j(0), sep_size(separator.size());
	register int k(0);
	stringArr arr;
	while (true)
	{
		if (limit!=-1)
		{
			if (k>limit)
				break;
			++k;
		}
		
		i = fields.find(separator,i);
		arr.push_back(escapeF(fields.substr(j,i-j)));
		if (i==std::string::npos)
			break;
			
		i += sep_size;
		j = i;
	}
	
	return arr;
}

//-------------------------------------------------------------------

stringArr 
tools::explode(const std::string &fields, 
			const std::string &separator,
			int limit)
{
	return explode(fields,&dummy,separator,limit);
}

//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		escape escapeF, 
		const std::string &separator,
		const std::string &frame,
		int limit)
{
	register int k(0);
	
	std::string temp, fs(frame + separator);
	stringArr::const_iterator i(fields.begin()), j(fields.end()-1);
	
	for (;i!=j;++i)
	{
		if (limit!=-1)
		{
			if (k>limit)
				return temp;
			++k;
		}		
		temp.append(frame + escapeF(*i) + fs);
	}
	temp.append(frame + escapeF(*i) + frame);
	
	return temp;
}
//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		const std::string &separator,
		const std::string &frame,
		int limit)
{	
	return implode(fields,&dummy,separator,frame,limit);
}

//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		const std::string &separator,
		int limit)
{
	return implode(fields,&dummy,separator,limit);
}

//-------------------------------------------------------------------

std::string 
tools::implode(const stringArr &fields, 
		escape escapeF, 
		const std::string &separator,
		int limit)
{
	register int k(0);	
	
	std::string temp;
	stringArr::const_iterator i(fields.begin()), j(fields.end()-1);
	
	for (;i!=j;++i)
	{
		if (limit!=-1)
		{
			if (k>limit)
				return temp;
			++k;
		}
		temp.append(escapeF(*i) + separator);
	}
	temp.append(escapeF(*i));
	
	return temp;	
}

//-------------------------------------------------------------------

std::string 
tools::lToString(long number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%ld",number);
	return temp;
}

//-------------------------------------------------------------------

std::string 
tools::dToString(double number)
{
	char temp[SIZEOFNUM];
	sprintf(temp,"%f",number);
	return temp;	
}

//-------------------------------------------------------------------

std::string 
tools::lTrim(const std::string &data, 
			char symbol)
{
	register int size = data.size(), i(0);
	
	for (;i<size;++i)
		if (data[i]!=symbol)
			break;
	
	return data.substr(i,size-i);		
}

//-------------------------------------------------------------------

std::string 
tools::rTrim(const std::string &data, 
			char symbol)
{
	register int i(data.size()-1);
	
	for (;i>=0;--i)
		if (data[i]!=symbol)
			break;
	
	return data.substr(0,i+1);	
}

//-------------------------------------------------------------------

std::string 
tools::rTrim(const std::string &data, 
			char symbols[], 
			int symCount)
{
	register int i(data.size()-1),j,empty;
	
	for (;i>=0;--i)
	{
		for (j=0,empty=0;j<symCount;++j)
			if (data[i]!=symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}
	
	return data.substr(0,i+1);
}

//-------------------------------------------------------------------

std::string 
tools::lTrim(const std::string &data, 
			char symbols[], 
			int symCount)
{
	register int size = data.size(),i(0),empty,j;
	
	for (;i<size;++i)
	{
		for (j=0,empty=0;j<symCount;++j)
			if (data[i]!=symbols[j])
				++empty;
		if (empty == symCount)
			break;
	}
	
	return data.substr(i,size-i);		
}

//-------------------------------------------------------------------

std::string 
tools::trim(const std::string &data, 
			char symbols[], 
			int symCount)
{
	return rTrim(lTrim(data,symbols,symCount),symbols,symCount);
}

//-------------------------------------------------------------------

std::string 
tools::trim(const std::string &data, 
			char symbol)
{
	return rTrim(lTrim(data,symbol),symbol);
}

//-------------------------------------------------------------------

#ifdef CODECONV_EXT

	#ifndef NO_EX
		void 
	#else
		bool 
	#endif		
	tools::codeSet(const std::string &toCode, 
					const std::string &fromCode)
	{
		conv = iconv_open(toCode.c_str(),fromCode.c_str());
		if (conv == (iconv_t)(-1))
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_CODESET,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		#ifdef NO_EX
			return true;
		#endif		
	}

	//-------------------------------------------------------------------
	
	std::string 
	tools::codesetConversion(const std::string &buffer, 
						const std::string &toCode, 
						const std::string &fromCode)
	{
		codeSet(toCode,fromCode);
		
		in = buffer.size();
		out = in*2;
		char *outBuffer = new char[out];
		if (outBuffer==NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_CODESETCONVERSION,ERR_LIBDODO,TOOLS_MEMORY_OVER,TOOLS_MEMORY_OVER_STR,__LINE__,__FILE__);
			#else
				return buffer;
			#endif
			
		
		inFake = (char *)buffer.c_str();
		outFake = outBuffer;
		
		if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
			#ifndef NO_EX
			{
				delete [] outBuffer;
				throw baseEx(ERRMODULE_TOOLS,TOOLS_CODESETCONVERSION,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			}
			#else
			{
				delete [] outBuffer;
				return buffer;
			}
			#endif
		
		result.assign(outBuffer, out);

		delete [] outBuffer;
		
		return result;
	}

	//-------------------------------------------------------------------
	
	std::string 
	tools::reCodesetConversion(const std::string &buffer)
	{
		if (!convSet)
			return buffer;
		
		in = buffer.size();
		out = in*2;
		char *outBuffer = new char[out];
		if (outBuffer == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_RECODESETCONVERSION,ERR_LIBDODO,TOOLS_MEMORY_OVER,TOOLS_MEMORY_OVER_STR,__LINE__,__FILE__);
			#else
				return buffer;
			#endif
					
		inFake = (char *)buffer.c_str();
		outFake = outBuffer;
		
		if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
			#ifndef NO_EX
			{
				delete [] outBuffer;
				throw baseEx(ERRMODULE_TOOLS,TOOLS_RECODESETCONVERSION,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			}
			#else
			{
				delete [] outBuffer;
				return buffer;
			}
			#endif
		
		result.assign(outFake, out);
		
		delete [] outBuffer;
		
		return result;			
	}
	
#endif	

//-------------------------------------------------------------------

	#ifdef ZLIB_EXT
	
		std::string 
		tools::zCompress(const std::string &buffer, 
						unsigned short level, 
						zlibCompressionStrategyEnum type)
		{
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			
			if ( (ret=deflateInit2(&strm,level,Z_DEFLATED,15,level,type))<0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_ZCOMPRESS,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
				#else
					return buffer;
				#endif
				
			strm.avail_in =  buffer.size();
			strm.next_in = (Bytef *)buffer.c_str();
			
			byteBuf = new Bytef[ZLIB_CHUNK];
			if (byteBuf == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_ZCOMPRESS,ERR_LIBDODO,TOOLS_MEMORY_OVER,TOOLS_MEMORY_OVER_STR,__LINE__,__FILE__);
			#else
				return buffer;
			#endif
			
			strBuf.clear();

			do
			{	
				strm.avail_out = ZLIB_CHUNK;
				strm.next_out = byteBuf;
			
				if ( (ret=deflate(&strm,Z_FINISH))<0)
					#ifndef NO_EX
					{
						delete [] byteBuf;
						throw baseEx(ERRMODULE_TOOLS,TOOLS_ZCOMPRESS,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
					}
					#else
					{
						delete [] byteBuf;
						return buffer;
					}
					#endif
					
				strBuf.append((char *)byteBuf,ZLIB_CHUNK-strm.avail_out);
			}
			while (strm.avail_out==0);
			
			deflateEnd(&strm);
			delete [] byteBuf;
			
			return strBuf;
		}

		//-------------------------------------------------------------------
	
		std::string 
		tools::zDecompress(const std::string &buffer)
		{
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			
			if ( (ret=inflateInit2(&strm,15))<0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_ZDECOMPRESS,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
				#else
					return buffer;
				#endif


			byteBuf = new Bytef[ZLIB_CHUNK];
			if (byteBuf == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_ZDECOMPRESS,ERR_LIBDODO,TOOLS_MEMORY_OVER,TOOLS_MEMORY_OVER_STR,__LINE__,__FILE__);
			#else
				return buffer;
			#endif

			strm.avail_in = buffer.size();
			strm.next_in = (Bytef *)buffer.c_str();
						
			strBuf.clear();

			do
			{	
				strm.avail_out = ZLIB_CHUNK;
				strm.next_out = byteBuf;
			
				if ( (ret=inflate(&strm,Z_NO_FLUSH))<0)
					#ifndef NO_EX
					{
						delete [] byteBuf;
						throw baseEx(ERRMODULE_TOOLS,TOOLS_ZDECOMPRESS,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
					}
					#else
					{
						delete [] byteBuf;
						return buffer;
					}
					#endif
					
				strBuf.append((char *)byteBuf,ZLIB_CHUNK-strm.avail_out);
			}
			while (strm.avail_out==0); 

			inflateEnd(&strm);
			delete [] byteBuf;
			
			return strBuf;
		}	
	#endif

//-------------------------------------------------------------------
