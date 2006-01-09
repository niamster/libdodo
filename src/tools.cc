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

/**
 * for ASCII85 decode
 */
static unsigned long powASCII85[] = {
								85*85*85*85, 
								85*85*85, 
								85*85, 
								85, 
								1
								};

//-------------------------------------------------------------------

inline std::string
tools::dummyTools(const std::string &data)
{
	return data;
}

//-------------------------------------------------------------------

tools::tools(tools &tls)
{
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

stringArr 
tools::explode(const std::string &fields, 
			const std::string &separator,
			int limit)
{
	return explode(fields,&dummyTools,separator,limit);
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
	return implode(fields,&dummyTools,separator,frame,limit);
}

//-------------------------------------------------------------------

std::string
tools::implode(const stringArr &fields,
		const std::string &separator,
		int limit)
{
	return implode(fields,&dummyTools,separator,limit);
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

	//-------------------------------------------------------------------
	
	std::string 
	tools::codesetConversionStatic(const std::string &buffer, 
						const std::string &toCode, 
						const std::string &fromCode)
	{
		iconv_t conv = iconv_open(toCode.c_str(),fromCode.c_str());
		if (conv == (iconv_t)(-1))
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_CODESETCONVERSIONSTATIC,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif

				
		register size_t in, out;
		register char *inFake, *outFake;
		
		in = buffer.size();
		out = in*2;
		char *outBuffer = new char[out];
				
		inFake = (char *)buffer.c_str();
		outFake = outBuffer;
		
		#ifdef BSD
			if (iconv(conv,(const char **)&inFake,&in,&outFake,&out) == (size_t)(-1))
		#else
			if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
		#endif
			#ifndef NO_EX
			{
				delete [] outBuffer;
				throw baseEx(ERRMODULE_TOOLS,TOOLS_CODESETCONVERSIONSTATIC,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			}
			#else
			{
				delete [] outBuffer;
				return buffer;
			}
			#endif
			
		std::string result;
		result.assign(outBuffer, out);

		delete [] outBuffer;
		
		return result;
	}
	
	//-------------------------------------------------------------------
	
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
				
		inFake = (char *)buffer.c_str();
		outFake = outBuffer;
		
		#ifdef BSD
			if (iconv(conv,(const char **)&inFake,&in,&outFake,&out) == (size_t)(-1))
		#else
			if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
		#endif
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
					
		inFake = (char *)buffer.c_str();
		outFake = outBuffer;
		
		#ifdef BSD
			if (iconv(conv,(const char **)&inFake,&in,&outFake,&out) == (size_t)(-1))
		#else
			if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
		#endif
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
	
		//-------------------------------------------------------------------
	
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
		
		//-------------------------------------------------------------------
		
		std::string 
		tools::zCompressStatic(const std::string &buffer, 
						unsigned short level, 
						zlibCompressionStrategyEnum type)
		{
		 	z_stream strm;
		 	int ret;
		 	
		 	std::string strBuf;
		 	Bytef *byteBuf;
			 				
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			
			if ( (ret=deflateInit2(&strm,level,Z_DEFLATED,15,level,type))<0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_ZCOMPRESSSTATIC,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
				#else
					return buffer;
				#endif
				
			strm.avail_in =  buffer.size();
			strm.next_in = (Bytef *)buffer.c_str();
			
			byteBuf = new Bytef[ZLIB_CHUNK];
			
			strBuf.clear();

			do
			{	
				strm.avail_out = ZLIB_CHUNK;
				strm.next_out = byteBuf;
			
				if ( (ret=deflate(&strm,Z_FINISH))<0)
					#ifndef NO_EX
					{
						delete [] byteBuf;
						throw baseEx(ERRMODULE_TOOLS,TOOLS_ZCOMPRESSSTATIC,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
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
		tools::zDecompressStatic(const std::string &buffer)
		{
		 	z_stream strm;
		 	int ret;
		 	
		 	std::string strBuf;
		 	Bytef *byteBuf;
		 				
			strm.zalloc = Z_NULL;
			strm.zfree = Z_NULL;
			strm.opaque = Z_NULL;
			
			if ( (ret=inflateInit2(&strm,15))<0)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_ZDECOMPRESSSTATIC,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
				#else
					return buffer;
				#endif


			byteBuf = new Bytef[ZLIB_CHUNK];

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
						throw baseEx(ERRMODULE_TOOLS,TOOLS_ZDECOMPRESSSTATIC,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
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
		
		//-------------------------------------------------------------------		
				
	#endif

//-------------------------------------------------------------------

char 
tools::hexToChar(const char first,
				const char second)
{
	int val=0;
	switch (first)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			val = (16*(int(first)-48));
			break;
			
		default:
			val = (16*(int(first)-55));
	}  
	
	switch (second)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			val += (int(second)-48);
		break;
		
		default:
			val += (int(second)-55);
	}   
	
	return char(val);
}

//-------------------------------------------------------------------

void
tools::charToHex(char result[3],
				const char first)
{
	sprintf(result,"%.2X",first);
}

//-------------------------------------------------------------------

std::string 
tools::decodeURL(const std::string &string)
{
	std::string result;
	register unsigned long o(0),k(string.size());

	for(;o<k;++o) 
	{
		switch(string[o]) 
		{
			case '+':
				result.append(1, ' ');
				break;
				
			case '%':
				if((k-o) >= 2 && std::isxdigit(string[o+1]) && std::isxdigit(string[o+2])) 
					result.append(1, tools::hexToChar(string[++o], string[++o]));
				else 
					result.append(1, '%');
				break;
				
			default:
				result.append(1, string[o]);
		}
	}
	
	return result;	
}

//-------------------------------------------------------------------

std::string
tools::encodeURL(const std::string &string)
{
	std::string result;
	
	register unsigned long i(0), j(string.size());
	register char temp[3];
	
	for(;i!=j;++i) 
	{
		switch(string[i]) 
	    {
		    case ' ':
				result.append(1, '+');
				break;
				
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G':
			case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
			case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U':
			case 'V': case 'W': case 'X': case 'Y': case 'Z':
			case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g':
			case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
			case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u':
			case 'v': case 'w': case 'x': case 'y': case 'z':
			case '0': case '1': case '2': case '3': case '4': case '5': case '6':
			case '7': case '8': case '9':
			case '-': case '_': case '.': case '~':
				result.append(1, string[i]);
				break;
				
			default:
				result.append(1, '%');
				tools::charToHex(temp,string[i]);
				result.append(temp);
				break;
	    }
	}
	
	return result;
}

//-------------------------------------------------------------------

void
tools::_encodeASCII85(std::string &result,
					unsigned long tuple, 
					int count)
{
	register short int i(5);
	register char buf[5], *s = buf;
	
	do 
	{
		*s++ = tuple % 85;
		tuple /= 85;
	} 
	while (--i > 0);
	
	i = count;
	do 
	{
		result.append(1,(char)(*--s + '!'));
	} 
	while (i-- > 0);
}

//-------------------------------------------------------------------

std::string 
tools::encodeASCII85(const std::string &string)
{
	std::string result("<~");
	register unsigned long tuple = 0;
	
	unsigned short count(0);
	register int j = string.size();
	
	for (register int k(0);k<j;++k)
	{
		switch (count++) 
		{
			case 0:	
				tuple |= (string[k] << 24); 
				break;
				
			case 1: 
				tuple |= (string[k] << 16); 
				break;
				
			case 2:	
				tuple |= (string[k] <<  8); 
				break;
				
			case 3:
				tuple |= string[k];
				
				if (tuple == 0)
					result.append(1,'z');
				else
					tools::_encodeASCII85(result, tuple, count);
					
				tuple = 0;
				count = 0;
				break;
		}
	}
	
	if (count > 0)
		tools::_encodeASCII85(result, tuple, count);
	
	result.append("~>");
	
	return result;
}

//-------------------------------------------------------------------

void 
tools::_decodeASCII85(std::string &result, 
					unsigned long tuple, 
					int count)
{
	switch (count) 
	{
		case 4:
			result.append(1,(char)(tuple >> 24));
			result.append(1,(char)(tuple >> 16));
			result.append(1,(char)(tuple >>  8));
			result.append(1,(char)(tuple));
			break;
			
		case 3:
			result.append(1,(char)(tuple >> 24));
			result.append(1,(char)(tuple >> 16));
			result.append(1,(char)(tuple >>  8));
			break;
			
		case 2:
			result.append(1,(char)(tuple >> 24));
			result.append(1,(char)(tuple >> 16));
			break;
			
		case 1:
			result.append(1,(char)(tuple >> 24));
			break;
		}	
}

//-------------------------------------------------------------------

std::string 
tools::decodeASCII85(const std::string &string)
{
	register unsigned long j = string.size(), count = 0, tuple = 0;
	register bool _break = false;
	std::string result;
	
	for (register unsigned long k(0);k<j;++k)
	{
		if (string[k] == '<')
		{
			if ((j-k) >= 1 && string[++k] == '~')
			{
				_break = false;
				while (!_break && k<j)
				{
					switch(string[++k])
					{
						default:
							if (string[k] < '!' || string[k] > 'u')
								#ifndef NO_EX
									throw baseEx(ERRMODULE_TOOLS,TOOLS_DECODEASCII85,ERR_LIBDODO,TOOLS_BAD_ASCII85,TOOLS_BAD_ASCII85_STR,__LINE__,__FILE__);
								#else
									return result;
								#endif
									
							tuple += (string[k] - '!') * powASCII85[count++];
							if (count == 5) 
							{
								_decodeASCII85(result,tuple,4);
								count = 0;
								tuple = 0;
							}
							break;
							
						case 'z':
							if (count != 0) 
								#ifndef NO_EX
									throw baseEx(ERRMODULE_TOOLS,TOOLS_DECODEASCII85,ERR_LIBDODO,TOOLS_BAD_ASCII85,TOOLS_BAD_ASCII85_STR,__LINE__,__FILE__);
								#else
									return result;
								#endif
							
							result.append(4,'\0');
							break;
							
						case '~':
							if ((k-j) >= 1 && string[++k] == '>') 
							{
								if (count > 0) 
								{
									count--;
									tuple += powASCII85[count];
									_decodeASCII85(result,tuple,count);
								}
								++k;
								_break = true;
								break;
							}
							
							#ifndef NO_EX
								throw baseEx(ERRMODULE_TOOLS,TOOLS_DECODEASCII85,ERR_LIBDODO,TOOLS_BAD_ASCII85,TOOLS_BAD_ASCII85_STR,__LINE__,__FILE__);
							#else
								return result;
							#endif
							
						case '\n': case '\r': case '\t': case ' ':
						case '\0': case '\f': case '\b': case 0177:
							break;						
					}
				}
			}
			else
			{
				result.append(1,'<');
				result.append(1,string[k]);
			}
		}
		else
			result.append(1,string[k]);
			
	}	
	
	return result;
}

//-------------------------------------------------------------------
