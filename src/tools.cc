/***************************************************************************
 *            tools.cc
 *
 *  Wed Aug 24 00:43:21 2005
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
 
#include <tools.h>
using namespace dodo;

/**
 * for MD5
 */

/**
 * magic numbers =)
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

//-------------------------------------------------------------------

static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))

//-------------------------------------------------------------------

#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))

//-------------------------------------------------------------------

#define H(x, y, z) ((x) ^ (y) ^ (z))

//-------------------------------------------------------------------

#define I(x, y, z) ((y) ^ ((x) | (~z)))

/**
 * ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/**
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) \
		{ \
			(a) += F ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		
//-------------------------------------------------------------------
  		
#define GG(a, b, c, d, x, s, ac) \
		{ \
			(a) += G ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
  		}
  		
//-------------------------------------------------------------------

#define HH(a, b, c, d, x, s, ac) \
		{ \
			(a) += H ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
  		}
  		
//-------------------------------------------------------------------

#define II(a, b, c, d, x, s, ac) \
		{ \
			(a) += I ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = ROTATE_LEFT ((a), (s)); \
			(a) += (b); \
		}
		
//-------------------------------------------------------------------

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

/**
 * for base64 encoding
 */
static const char base64EncodeTr[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//-------------------------------------------------------------------

/**
 * for base64 decoding
 */
static const char base64DecodeTr[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

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
	register unsigned long i(0), j(0), sep_size(separator.size());
	register int k(0);
	stringArr arr;
	
	while (true)
	{
		if (limit != -1)
		{
			if (k>limit)
				break;
			++k;
		}
		
		i = fields.find(separator,i);
		arr.push_back(escapeF(fields.substr(j,i-j)));
		if (i == std::string::npos)
			break;
			
		i += sep_size;
		j = i;
	}
	
	return arr;
}

//-------------------------------------------------------------------

void 
tools::replace(const std::string &needle, 
		const std::string &replacement,
		std::string &data)
{
	register unsigned long i(0),j(needle.size()),k(replacement.size());
	
	while (true)
	{
		i = data.find(needle,i);
		if (i == std::string::npos)
			break;
			
		data.replace(i,j,replacement,0,k);
		i += k;
	}
}

//-------------------------------------------------------------------

void 
tools::replace(const stringArr &needle, 
		const stringArr &replacement,
		std::string &data)
{
	stringArr::const_iterator i = needle.begin(), j = needle.end(), o = replacement.begin(), p = replacement.end();
	for (;i!=j&&o!=p;++i,++o)
		replace(*i, *o, data);
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
	stringArr::const_iterator i(fields.begin()), j(--fields.end());
	
	for (;i!=j;++i)
	{
		if (limit != -1)
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
	stringArr::const_iterator i(fields.begin()), j(--fields.end());
	
	for (;i!=j;++i)
	{
		if (limit != -1)
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
		if (data[i] != symbol)
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
		if (data[i] != symbol)
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
			if (data[i] != symbols[j])
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
			if (data[i] != symbols[j])
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

#ifdef ICONV_EXT

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
		
		#ifdef __FreeBSD__
			if (iconv(conv,(const char **)&inFake,&in,&outFake,&out) == (size_t)(-1))
		#else
			if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
		#endif
		{
			delete [] outBuffer;
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_CODESETCONVERSIONSTATIC,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return buffer;
			#endif
		}
			
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
		
		#ifdef __FreeBSD__
			if (iconv(conv,(const char **)&inFake,&in,&outFake,&out) == (size_t)(-1))
		#else
			if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
		#endif
		{
			delete [] outBuffer;
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_CODESETCONVERSION,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return buffer;
			#endif
		}
		
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
		
		#ifdef __FreeBSD__
			if (iconv(conv,(const char **)&inFake,&in,&outFake,&out) == (size_t)(-1))
		#else
			if (iconv(conv,&inFake,&in,&outFake,&out) == (size_t)(-1))
		#endif
			{
				delete [] outBuffer;
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_RECODESETCONVERSION,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return buffer;
				#endif
			}
		
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
					short type)
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
			{
				delete [] byteBuf;
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_ZCOMPRESS,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
				#else
					return buffer;
				#endif
			}
				
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
	 	z_stream strm;
	 	int ret;
	 	
	 	std::string strBuf;
	 	Bytef *byteBuf;
	 				
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
			{
				delete [] byteBuf;
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_ZDECOMPRESS,ERR_ZLIB,ret,strm.msg==NULL?"":strm.msg,__LINE__,__FILE__);
				#else
					return buffer;
				#endif
			}
				
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
	register int val=0;
	
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
				{
					result.append(1, tools::hexToChar(string[o+1], string[o+2]));
					o += 2;
				}
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
				
			case 'A': 
			case 'B': 
			case 'C': 
			case 'D': 
			case 'E': 
			case 'F': 
			case 'G':
			case 'H': 
			case 'I': 
			case 'J': 
			case 'K': 
			case 'L': 
			case 'M': 
			case 'N':
			case 'O': 
			case 'P': 
			case 'Q': 
			case 'R': 
			case 'S': 
			case 'T': 
			case 'U':
			case 'V': 
			case 'W': 
			case 'X': 
			case 'Y': 
			case 'Z':
			case 'a': 
			case 'b': 
			case 'c': 
			case 'd': 
			case 'e': 
			case 'f': 
			case 'g':
			case 'h': 
			case 'i': 
			case 'j': 
			case 'k': 
			case 'l': 
			case 'm': 
			case 'n':
			case 'o': 
			case 'p': 
			case 'q': 
			case 'r': 
			case 's': 
			case 't': 
			case 'u':
			case 'v': 
			case 'w': 
			case 'x': 
			case 'y': 
			case 'z':
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
			case '-': 
			case '_': 
			case '.': 
			case '~':
			
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
							
						case '\n': 
						case '\r': 
						case '\t': 
						case ' ':
						case '\0': 
						case '\f': 
						case '\b': 
						case 0177:
						
							break;	
							
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

void 
tools::_encodeBase64(unsigned char in[3], 
					unsigned char out[4], 
					int len)
{
    out[0] = base64EncodeTr[in[0] >> 2];
    out[1] = base64EncodeTr[((in[0]&0x03) << 4) | ((in[1]&0xf0) >> 4)];
    out[2] = (unsigned char)(len>1?base64EncodeTr[((in[1]&0x0f) << 2) | ((in[2]&0xc0) >> 6) ]:'=');
    out[3] = (unsigned char)(len>2?base64EncodeTr[in[2]&0x3f ]:'=');    
}

//-------------------------------------------------------------------

std::string 
tools::encodeBase64(const std::string &string)
{
	register unsigned long j = string.size();
	register unsigned char in[3], out[4];
	register unsigned short i, len;
	std::string result;
	
	for (register unsigned long k(0);k<j;)
	{
        len = 0;
        for(i=0;i<3;++i) 
        {
        	++k;
        	if (k <= j)
        	{
				in[i] = string[k-1];
				++len;
        	}
            else
                in[i] = 0;
        }
        
        if(len > 0) 
        {
            _encodeBase64(in,out,len);
            for(i=0;i<4;++i) 
                result.append(1,out[i]);
        }
	}
	
	result.append("\r\n");
	
	return result;
}

//-------------------------------------------------------------------

void 
tools::_decodeBase64(unsigned char in[4], 
					unsigned char out[3])
{
    out[0] = (unsigned char )(in[0] << 2 | in[1] >> 4);
    out[1] = (unsigned char )(in[1] << 4 | in[2] >> 2);
    out[2] = (unsigned char )(((in[2] << 6) & 0xc0) | in[3]);	
}

//-------------------------------------------------------------------

std::string 
tools::decodeBase64(const std::string &string)
{
	register unsigned long j = string.size();	
    register unsigned char in[4], out[3], v;
    register unsigned short i, len;
	std::string result;
	
	for (register unsigned long k(0);k<j;)
	{
		len = 0;
        for(len=0,i=0;i<4 && k<j;++i) 
        {
            v = 0;
            while(k < j && v == 0) 
            {
                v = string[k++];
                v = (unsigned char)((v<43||v>122)?0:base64DecodeTr[v-43]);
                if(v) 
                    v = (unsigned char)((v == '$')?0:v-61);
            }
            
            if(k < j) 
            {
                ++len;
                if(v)
					in[i] = (unsigned char)(v-1);
            }
            else
                in[i] = 0;
        }
        
        if(len > 0) 
        {
            _decodeBase64(in,out);
            for( i = 0; i < len-1;++i)
            	result.append(1,out[i]);
        }
	}
	
	return result;
}

//-------------------------------------------------------------------

__url 
tools::parseURL(const std::string &url)
{
	register unsigned long begin(0), pos, pos1;
	
	__url temp;
	
	if ((pos = url.find("://",0)) != std::string::npos)
	{
		temp.protocol = url.substr(0,pos);
		
		begin = pos + 3;
	}
	
	if ((pos = url.find('@',begin)) != std::string::npos)
	{
		if ((pos1 = url.find(':',begin)) < pos)
		{
			temp.login = url.substr(begin,pos1 - begin);
			
			++pos1;
			
			temp.password = url.substr(pos1,pos - pos1);
		}
		else	
			temp.login = url.substr(begin,pos - begin);
		
		begin = pos + 1;	
	}

	if ((pos = url.find('/',begin)) != std::string::npos)
	{
		if ((pos1 = url.find(':',begin)) < pos)
		{
			temp.host = url.substr(begin,pos1 - begin);
			
			++pos1;
			
			temp.port = url.substr(pos1,pos - pos1);
		}
		else	
			temp.host = url.substr(begin,pos - begin);
		
		begin = pos + 1;	

		if ((pos = url.find('?',begin)) != std::string::npos)
		{
			temp.path = url.substr(begin,pos - begin);
			temp.request = url.substr(pos+1);			
		}
		else
			temp.path = url.substr(begin);
	}
	else
	{
		if ((pos1 = url.find(':',begin)) < pos)
		{
			temp.host = url.substr(begin,pos1 - begin);
			
			++pos1;
			
			temp.port = url.substr(pos1,pos - pos1);
		}
		else	
			temp.host = url.substr(begin,pos - begin);		
	}
	
	return temp;
}

//-------------------------------------------------------------------

#ifdef BZIP2_EXT

	//-------------------------------------------------------------------

	std::string 
	tools::bzCompress(const std::string &buffer, 
					unsigned short level, 
					unsigned short type)
	{		
		unsigned int len = buffer.size();
		char *dst = new char[len + 1];
		
		register int ret = BZ2_bzBuffToBuffCompress(dst, &len, (char *)buffer.c_str(), len, level, 0, type);
		if (ret != BZ_OK)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_BZCOMPRESS,ERR_BZIP,TOOLS_BAD_BZCOMPRESSION,TOOLS_BAD_BZCOMPRESSION_STR,__LINE__,__FILE__);
			#else
				return __string__;
			#endif		
		
		return std::string(dst,len);		
	}					

	//-------------------------------------------------------------------

	std::string 
	tools::bzDecompress(const std::string &buffer)
	{
	    bz_stream bzs;
	
	    bzs.bzalloc = NULL;
	    bzs.bzfree = NULL;
	
	    register int ret = BZ2_bzDecompressInit(&bzs, 0, 0);
		if (ret != BZ_OK)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_BZDECOMPRESS,ERR_BZIP,TOOLS_BAD_BZDECOMPRESSION_INIT,TOOLS_BAD_BZDECOMPRESSION_INIT_STR,__LINE__,__FILE__);
			#else
				return __string__;
			#endif

	    int src_len = buffer.size();
	    char *src = new char[src_len + 1];
	    memcpy(src,buffer.c_str(),src_len);
	    
	    unsigned long long size = 0;

		bzs.avail_out = BZIP_CHUNK;
	    char *dst = (char *)malloc(bzs.avail_out + 1);
		bzs.next_out = dst;
	
	    bzs.next_in = src;
	    bzs.avail_in = src_len;
	
		std::string _buffer;
	
	    while ((ret = BZ2_bzDecompress(&bzs)) == BZ_OK && bzs.avail_in > 0)
	    {
	        bzs.avail_out = src_len;
	        size = (bzs.total_out_hi32 * (unsigned int) -1) + bzs.total_out_lo32;
	        dst = (char *)realloc(dst, size + bzs.avail_out + 1);
	        bzs.next_out = dst + size;
	    }
	
	    if (ret == BZ_STREAM_END || ret == BZ_OK) 
	    {
			size = (bzs.total_out_hi32 * (unsigned int) -1) + bzs.total_out_lo32;
			_buffer.assign(dst, size);
			
			delete [] src;
			free(dst);
	    } 
	    else 
	    {
	    	delete [] src;
	        free(dst);
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_BZDECOMPRESS,ERR_BZIP,TOOLS_BAD_BZDECOMPRESSION,TOOLS_BAD_BZDECOMPRESSION_STR,__LINE__,__FILE__);
			#else
				return __string__;
			#endif	        
	    }
	
	    ret = BZ2_bzDecompressEnd(&bzs);
		if (ret != BZ_OK)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_TOOLS,TOOLS_BZDECOMPRESS,ERR_BZIP,TOOLS_BAD_BZDECOMPRESSION_FINISH,TOOLS_BAD_BZDECOMPRESSION_FINISH_STR,__LINE__,__FILE__);
			#else
				return __string__;
			#endif	    
			
		return _buffer;
	}

	//-------------------------------------------------------------------

#endif

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
tools::mail(const std::string &path,
			const std::string &to, 
			const std::string &subject, 
			const std::string &message, 
			const std::string &headers)
{
	FILE *sendmail = popen((path + " " + to).c_str(), "w");

	if (sendmail == NULL) 
		#ifndef NO_EX
			throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	fprintf(sendmail, "To: %s\n", to.c_str());
	fprintf(sendmail, "Subject: %s\n", subject.c_str());
	if (headers.size() > 0)
		fprintf(sendmail, "%s\n", headers.c_str());
	fprintf(sendmail, "\n%s\n", message.c_str());

	if (pclose(sendmail) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif

	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

void 
tools::MD5Init(MD5_CTX *context)
{
  context->count[0] = context->count[1] = 0;

  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

//-------------------------------------------------------------------

void 
tools::MD5Transform(unsigned int state[4], 
					unsigned char block[64])
{
	unsigned int a = state[0], b = state[1], c = state[2], d = state[3], x[16];

	for (unsigned int i = 0, j = 0; j < 64; ++i, j += 4)
		x[i] = ((unsigned int)block[j]) | (((unsigned int)block[j+1]) << 8) | (((unsigned int)block[j+2]) << 16) | (((unsigned int)block[j+3]) << 24);

	FF (a, b, c, d, x[ 0], S11, 0xd76aa478);
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756);
	FF (c, d, a, b, x[ 2], S13, 0x242070db);
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee);
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf);
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a);
	FF (c, d, a, b, x[ 6], S13, 0xa8304613);
	FF (b, c, d, a, x[ 7], S14, 0xfd469501);
	FF (a, b, c, d, x[ 8], S11, 0x698098d8);
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af);
	FF (c, d, a, b, x[10], S13, 0xffff5bb1);
	FF (b, c, d, a, x[11], S14, 0x895cd7be);
	FF (a, b, c, d, x[12], S11, 0x6b901122);
	FF (d, a, b, c, x[13], S12, 0xfd987193);
	FF (c, d, a, b, x[14], S13, 0xa679438e);
	FF (b, c, d, a, x[15], S14, 0x49b40821);

	GG (a, b, c, d, x[ 1], S21, 0xf61e2562); 
	GG (d, a, b, c, x[ 6], S22, 0xc040b340);
	GG (c, d, a, b, x[11], S23, 0x265e5a51);
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d);
	GG (d, a, b, c, x[10], S22,  0x2441453);
	GG (c, d, a, b, x[15], S23, 0xd8a1e681);
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6);
	GG (d, a, b, c, x[14], S22, 0xc33707d6);
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87);
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed);
	GG (a, b, c, d, x[13], S21, 0xa9e3e905);
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8);
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9);
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);

	HH (a, b, c, d, x[ 5], S31, 0xfffa3942);
	HH (d, a, b, c, x[ 8], S32, 0x8771f681);
	HH (c, d, a, b, x[11], S33, 0x6d9d6122);
	HH (b, c, d, a, x[14], S34, 0xfde5380c);
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44);
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9);
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60);
	HH (b, c, d, a, x[10], S34, 0xbebfbc70);
	HH (a, b, c, d, x[13], S31, 0x289b7ec6);
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa);
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085);
	HH (b, c, d, a, x[ 6], S34,  0x4881d05);
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039);
	HH (d, a, b, c, x[12], S32, 0xe6db99e5);
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8);
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665);

  	II (a, b, c, d, x[ 0], S41, 0xf4292244);
  	II (d, a, b, c, x[ 7], S42, 0x432aff97);
	II (c, d, a, b, x[14], S43, 0xab9423a7);
	II (b, c, d, a, x[ 5], S44, 0xfc93a039);
	II (a, b, c, d, x[12], S41, 0x655b59c3);
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92);
	II (c, d, a, b, x[10], S43, 0xffeff47d);
	II (b, c, d, a, x[ 1], S44, 0x85845dd1);
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f);
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0);
	II (c, d, a, b, x[ 6], S43, 0xa3014314);
	II (b, c, d, a, x[13], S44, 0x4e0811a1);
	II (a, b, c, d, x[ 4], S41, 0xf7537e82);
	II (d, a, b, c, x[11], S42, 0xbd3af235);
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
	II (b, c, d, a, x[ 9], S44, 0xeb86d391);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

//-------------------------------------------------------------------

void 
tools::MD5Final(unsigned char digest[16], 
				MD5_CTX *context)
{	
	unsigned char bits[8];
	unsigned int index, padLen;
	
	unsigned int i, j;
	
	for (i = 0, j = 0; j < 8; ++i, j += 4) 
	{
		bits[j] = (unsigned char)(context->count[i] & 0xff);
		bits[j+1] = (unsigned char)((context->count[i] >> 8) & 0xff);
		bits[j+2] = (unsigned char)((context->count[i] >> 16) & 0xff);
		bits[j+3] = (unsigned char)((context->count[i] >> 24) & 0xff);
	}

	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update(context, PADDING, padLen);

	MD5Update(context, bits, 8);
  	
	for (i = 0, j = 0; j < 16; ++i, j += 4) 
	{
		digest[j] = (unsigned char)(context->state[i] & 0xff);
		digest[j+1] = (unsigned char)((context->state[i] >> 8) & 0xff);
		digest[j+2] = (unsigned char)((context->state[i] >> 16) & 0xff);
		digest[j+3] = (unsigned char)((context->state[i] >> 24) & 0xff);
	}
  
	memset(context, 0, sizeof(MD5_CTX));
}

//-------------------------------------------------------------------

void 
tools::MD5Update(MD5_CTX *context, 
				unsigned char *input, 
				unsigned int inputLen)
{
	unsigned int i, index, partLen;

	index = (unsigned int )((context->count[0] >> 3) & 0x3F);

	if ((context->count[0] += (inputLen << 3)) < (inputLen << 3))
		++context->count[1];
  	
	context->count[1] += (inputLen >> 29);
	
	partLen = 64 - index;

	if (inputLen >= partLen) 
	{
		memcpy(&context->buffer[index], input, partLen);
			
 		MD5Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform(context->state, &input[i]);

		index = 0;
	}
  	else
		i = 0;
		
	memcpy(&context->buffer[index], &input[i], inputLen - i);
}

//-------------------------------------------------------------------

std::string 
tools::MD5(const std::string &string)
{
	MD5_CTX context;
	unsigned char digest[16];
	
	MD5Init(&context);
	MD5Update(&context, (unsigned char *)string.c_str(), string.length());
	MD5Final(digest, &context);
	
	return std::string((char *)digest,16);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
tools::mail(const std::string &host,
			short type,
			int port,
			const std::string &to, 
			const std::string &from, 
			const std::string &subject, 
			const std::string &message,
			const std::string &login, 
			const std::string &pass,
			const std::string &headers)
{
	enum authTypeEnum
	{
		SMTPAUTH_CRAMMD5 = 2,
		SMTPAUTH_LOGIN = 4,
		SMTPAUTH_PLAIN = 8
	};
	
	unsigned short authType = 0;
	
	bool auth = (login.length()>0)?true:false;
	
	flushSocket fsock(false,type,TRANSFER_TYPE_STREAM);
	
	flushSocketExchange ex;
	
	fsock.connect(host,port,ex);
	
	std::string mess;
	regexpTools reg;
	stringArr pock;
	register bool matched = false;
	register int code = 0;
	
	reg.multiline = true;
	reg.compile("(\\d+)(.)(.*)");
	
	ex.receiveStreamString(mess);
	
	ex.sendStreamString("EHLO " + flushSocketTools::getLocalName() + "\r");

	ex.receiveStreamString(mess);
	matched = reg.reMatch(mess,pock);
	code = atoi(pock[0].c_str());
	
	if (!matched || code != 250)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,TOOLS_BADMAILHELO,TOOLS_BADMAILHELO_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	if (auth)	
	{
		if (strcasestr(pock[2].c_str(),"CRAM-MD5") != NULL)
			addF(authType,SMTPAUTH_CRAMMD5);
			
		if (strcasestr(pock[2].c_str(),"LOGIN") != NULL)
			addF(authType,SMTPAUTH_LOGIN);
			
		if (strcasestr(pock[2].c_str(),"PLAIN") != NULL)
			addF(authType,SMTPAUTH_PLAIN);
	}
	
	if (auth)
	{
		if ((SMTPAUTH_CRAMMD5&authType) == SMTPAUTH_CRAMMD5)
		{
			ex.sendStreamString("AUTH CRAM-MD5\r");
			ex.receiveStreamString(mess);
			matched = reg.reMatch(mess,pock);
			code = atoi(pock[0].c_str());
			
			if (!matched || code != 334)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,TOOLS_BADMAILAUTH,TOOLS_BADMAILAUTH_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			std::string ticket = decodeBase64(pock[2]);
			        
			std::string md5pass;
			if (pass.length() > 64)
				md5pass = MD5(pass);
        	else
        		md5pass = pass;
			
			unsigned char ipad[65];
        	unsigned char opad[65];
        	
        	memcpy(ipad,md5pass.c_str(),md5pass.length());
        	memcpy(opad,md5pass.c_str(),md5pass.length());
        	
			for (short i=0; i<64; ++i) 
			{
				ipad[i] ^= 0x36;
				opad[i] ^= 0x5c;
        	}
        	
        	MD5_CTX context;
			unsigned char digest[16];
        	
			MD5Init(&context);
			MD5Update(&context, ipad, 64);
			MD5Update(&context, (unsigned char *)ticket.c_str(), ticket.length());
			MD5Final(digest, &context);

			MD5Init(&context);
			MD5Update(&context, opad, 64);
			MD5Update(&context, digest, 16);
			MD5Final(digest, &context);      	
			
			md5pass.clear();
			for (short i = 0; i < 16; ++i)
			{
 				sprintf((char *)ipad,"%02x", digest[i]);
				md5pass.append((char *)ipad);
			}
	
			ex.sendStreamString(encodeBase64(login + " " + md5pass) + "\r");
			ex.receiveStreamString(mess);
			matched = reg.reMatch(mess,pock);
			code = atoi(pock[0].c_str());
			
			if (!matched || code != 334)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,TOOLS_BADMAILAUTH,TOOLS_BADMAILAUTH_STR,__LINE__,__FILE__);
				#else
					return false;
				#endif
		}
		else
		{
			if ((SMTPAUTH_LOGIN&authType) == SMTPAUTH_LOGIN)
			{
				ex.sendStreamString("AUTH LOGIN\r");
				ex.receiveStreamString(mess);
				matched = reg.reMatch(mess,pock);
				code = atoi(pock[0].c_str());
				
				if (!matched || code != 334)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,TOOLS_BADMAILAUTH,TOOLS_BADMAILAUTH_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
				
				ex.sendStreamString(encodeBase64(login) + "\r");
				ex.receiveStreamString(mess);
				matched = reg.reMatch(mess,pock);
				code = atoi(pock[0].c_str());
				
				if (!matched || code != 334)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,TOOLS_BADMAILAUTH,TOOLS_BADMAILAUTH_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
				
				ex.sendStreamString(encodeBase64(pass) + "\r");
				ex.receiveStreamString(mess);
				matched = reg.reMatch(mess,pock);
				code = atoi(pock[0].c_str());
				
				if (!matched || code != 235)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,TOOLS_BADMAILAUTH,TOOLS_BADMAILAUTH_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
			else
			{
				if ((SMTPAUTH_PLAIN&authType) == SMTPAUTH_PLAIN)
				{
					ex.sendStreamString("AUTH PLAIN" + encodeBase64(login + "\0" + login + "\0" + pass) + "\r");
					ex.receiveStreamString(mess);
					matched = reg.reMatch(mess,pock);
					code = atoi(pock[0].c_str());
					
					if (!matched || code != 334)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_TOOLS,TOOLS_MAIL,ERR_ERRNO,TOOLS_BADMAILAUTH,TOOLS_BADMAILAUTH_STR,__LINE__,__FILE__);
						#else
							return false;
						#endif
				}
			}
		}
	}
	
	ex.sendStreamString("MAIL FROM: <" + from + ">\r");
	ex.receiveStreamString(mess);
		
	pock = explode(to,",");
	
	stringArr::iterator i = pock.begin(), j = pock.end();
	for (;i!=j;++i)
	{
		ex.sendStreamString("RCPT TO: <" + *i + ">\r");
		ex.receiveStreamString(mess);
	}
	
	ex.sendStreamString("DATA\r\n");
	ex.receiveStreamString(mess);
	
	ex.sendStreamString("To: " + to + "\r");
	ex.sendStreamString("From: " + from + "\r");
	ex.sendStreamString("X-Mailer: libdodo\r");
	ex.sendStreamString("Subject: " + subject  + "\r");
	ex.sendStreamString(headers);
	ex.sendStreamString(message);
	ex.sendStreamString("\r\n.\r");
	ex.sendStreamString("QUIT\r");
	
	ex.close();
	
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------
