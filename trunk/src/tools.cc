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

#include <libdodo/tools.h>

using namespace dodo;

/**
 * for MD5
 */

/**
 * magic numbers =)
 */
#define S11    7
#define S12    12
#define S13    17
#define S14    22
#define S21    5
#define S22    9
#define S23    14
#define S24    20
#define S31    4
#define S32    11
#define S33    16
#define S34    23
#define S41    6
#define S42    10
#define S43    15
#define S44    21

//-------------------------------------------------------------------

static unsigned char PADDING[] = {
	0x80, 0,	0,    0,	0,    0,	0,    0,	0,    0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,    0,	0,    0,	0,    0,	0,    0,	0,    0,	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0,    0,	0,    0,	0,    0,	0,    0,	0,    0,	0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z)           (((x) & (y)) | ((~x) & (z)))

//-------------------------------------------------------------------

#define G(x, y, z)           (((x) & (z)) | ((y) & (~z)))

//-------------------------------------------------------------------

#define H(x, y, z)           ((x) ^ (y) ^ (z))

//-------------------------------------------------------------------

#define I(x, y, z)           ((y) ^ ((x) | (~z)))

/**
 * ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n)    (((x) << (n)) | ((x) >> (32 - (n))))

/**
 * FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
 * Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac)							\
	{														\
		(a) += F((b), (c), (d)) + (x) + (unsigned int)(ac);	\
		(a) = ROTATE_LEFT((a), (s));						\
		(a) += (b);											\
	}

//-------------------------------------------------------------------

#define GG(a, b, c, d, x, s, ac)							\
	{														\
		(a) += G((b), (c), (d)) + (x) + (unsigned int)(ac);	\
		(a) = ROTATE_LEFT((a), (s));						\
		(a) += (b);											\
	}

//-------------------------------------------------------------------

#define HH(a, b, c, d, x, s, ac)							\
	{														\
		(a) += H((b), (c), (d)) + (x) + (unsigned int)(ac);	\
		(a) = ROTATE_LEFT((a), (s));						\
		(a) += (b);											\
	}

//-------------------------------------------------------------------

#define II(a, b, c, d, x, s, ac)							\
	{														\
		(a) += I((b), (c), (d)) + (x) + (unsigned int)(ac);	\
		(a) = ROTATE_LEFT((a), (s));						\
		(a) += (b);											\
	}

//-------------------------------------------------------------------

/**
 * for ASCII85 decode
 */
static unsigned long powASCII85[] = {
	85 * 85 * 85 * 85,
	85 * 85 * 85,
	85 * 85,
	85,
	1
};

//-------------------------------------------------------------------

/**
 * for base64 encoding
 */
static const char base64EncodeTr[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//-------------------------------------------------------------------

/**
 * for base64 decoding
 */
static const char base64DecodeTr[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

//-------------------------------------------------------------------

inline dodoString
tools::dummyTools(const dodoString &data)
{
	return data;
}

//-------------------------------------------------------------------

void
tools::random(void *data,
			  unsigned long size,
			  short strength)
{
	FILE *file;

	if (strength == TOOLS_RANDOMSTRENGTH_DEFAULT)
	{
		file = fopen("/dev/urandom", "r");
		if (file == NULL)
			throw baseEx(ERRMODULE_TOOLS, TOOLSEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
	else
	{
		if (strength == TOOLS_RANDOMSTRENGTH_STRONG)
		{
			file = fopen("/dev/random", "r");
			if (file == NULL)
				throw baseEx(ERRMODULE_TOOLS, TOOLSEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
		}
		else
			throw baseEx(ERRMODULE_TOOLS, TOOLSEX_RANDOM, ERR_LIBDODO, TOOLSEX_WRONGSTRENGTH, TOOLSEX_WRONGSTRENGTH_STR, __LINE__, __FILE__);
	}
	
    while (true)
    {
            if (fread(data, size, 1, file) == 0)
            {
                    if (errno == EINTR)
                    	continue;

					if (errno == EAGAIN)
						break;

                    if (ferror(file) != 0)
                    	throw baseEx(ERRMODULE_TOOLS, TOOLSEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
            }

            break;
    }

	if (fclose(file) != 0)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_RANDOM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

dodoString
tools::stringRandom(unsigned long size,
					short strength)
{
	char *data = new char[size + 1];
	try
	{
		random(data, size, strength);
	}
	catch (...)
	{
		delete [] data;

		throw;
	}
	for (unsigned long i = 0; i < size; ++i)
		if (data[i] == '\0')
			data[i] = '*';

	dodoString res(data, size);
	delete [] data;

	return res;
}

//-------------------------------------------------------------------

unsigned long
tools::ulRandom(short strength)
{
	unsigned long rnd;

	random(&rnd, sizeof(unsigned long), strength);

	return rnd;
}

//-------------------------------------------------------------------

long
tools::lRandom(short strength)
{
	long rnd;

	random(&rnd, sizeof(long), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned int
tools::uiRandom(short strength)
{
	unsigned int rnd;

	random(&rnd, sizeof(unsigned int), strength);

	return rnd;
}

//-------------------------------------------------------------------

int
tools::iRandom(short strength)
{
	int rnd;

	random(&rnd, sizeof(int), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned short
tools::usRandom(short strength)
{
	unsigned short rnd;

	random(&rnd, sizeof(unsigned short), strength);

	return rnd;
}

//-------------------------------------------------------------------

short
tools::sRandom(short strength)
{
	short rnd;

	random(&rnd, sizeof(short), strength);

	return rnd;
}

//-------------------------------------------------------------------

unsigned char
tools::ucRandom(short strength)
{
	unsigned char rnd;

	random(&rnd, sizeof(unsigned char), strength);

	return rnd;
}

//-------------------------------------------------------------------

char
tools::cRandom(short strength)
{
	char rnd;

	random(&rnd, sizeof(char), strength);

	return rnd;
}

//-------------------------------------------------------------------

double
tools::dRandom(short strength)
{
	double rnd;

	random(&rnd, sizeof(double), strength);

	return rnd;
}

//-------------------------------------------------------------------

bool
tools::isInArray(const dodoStringArray &arr,
				 const dodoString &needle,
				 bool icase)
{
	bool (*cmpFunc)(const dodoString &, const dodoString &);

	if (icase)
		cmpFunc = stringTools::iequal;
	else
		cmpFunc = stringTools::equal;

	dodoStringArray::const_iterator i(arr.begin()), j(arr.end());
	for (; i != j; ++i)
		if (cmpFunc(*i, needle))
			return true;

	return false;
}

//-------------------------------------------------------------------

dodoStringArray
tools::explode(const dodoString &fields,
			   escape escapeF,
			   const dodoString &separator,
			   int limit)
{
	unsigned long i(0), j(0), sep_size(separator.size());
	int k(1);
	dodoStringArray arr;

	while (true)
	{
		if (limit != -1)
		{
			if (k > limit)
			{
				arr.back().append(escapeF(fields.substr(j - sep_size)));
				
				break;
			}
			++k;
		}

		i = fields.find(separator, i);
		arr.push_back(escapeF(fields.substr(j, i - j)));
		if (i == dodoString::npos)
			break;

		i += sep_size;
		j = i;
	}

	return arr;
}

//-------------------------------------------------------------------

dodoStringArray
tools::explode(const dodoString &fields,
			   const dodoString &separator,
			   int limit)
{
	return explode(fields, &dummyTools, separator, limit);
}

//-------------------------------------------------------------------

dodoString
tools::implode(const dodoStringArray &fields,
			   escape escapeF,
			   const dodoString &separator,
			   const dodoString &frame,
			   int limit)
{
	if (fields.size() == 0)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_IMPLODE, ERR_LIBDODO, TOOLSEX_EMPTYARRAY, TOOLSEX_EMPTYARRAY_STR, __LINE__, __FILE__);

	int k(0);

	dodoString temp, fs(frame + separator);
	dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			if (limit != -1)
			{
				if (k > limit)
					return temp;
				++k;
			}
			temp.append(frame + escapeF(*i) + fs);
		}
		temp.append(frame + escapeF(*i) + frame);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
tools::implode(const dodoStringArray &fields,
			   const dodoString &separator,
			   const dodoString &frame,
			   int limit)
{
	return implode(fields, &dummyTools, separator, frame, limit);
}

//-------------------------------------------------------------------

dodoString
tools::implode(const dodoStringArray &fields,
			   const dodoString &separator,
			   int limit)
{
	return implode(fields, &dummyTools, separator, limit);
}

//-------------------------------------------------------------------

dodoString
tools::implode(const dodoStringArray &fields,
			   escape escapeF,
			   const dodoString &separator,
			   int limit)
{
	if (fields.size() == 0)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_IMPLODE, ERR_LIBDODO, TOOLSEX_EMPTYARRAY, TOOLSEX_EMPTYARRAY_STR, __LINE__, __FILE__);

	int k(0);

	dodoString temp;
	dodoStringArray::const_iterator i(fields.begin()), j(fields.end());
	if (i != j)
	{
		--j;
		for (; i != j; ++i)
		{
			if (limit != -1)
			{
				if (k > limit)
					return temp;
				++k;
			}
			temp.append(escapeF(*i) + separator);
		}
		temp.append(escapeF(*i));
	}

	return temp;
}

//-------------------------------------------------------------------

#ifdef ICONV_EXT

dodoString
tools::codesetConversion(const dodoString &buffer,
						 const dodoString &toCode,
						 const dodoString &fromCode)
{
	iconv_t conv = iconv_open(toCode.c_str(), fromCode.c_str());
	if (conv == (iconv_t)(-1))
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_CODESETCONVERSION, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	size_t in, out, outBefore;
	char *inFake, *outFake;

	in = buffer.size();
	outBefore = out = (in + 1) * 4;
	char *outBuffer = new char[out];

	inFake = (char *)buffer.c_str();
	outFake = outBuffer;

#ifdef __FreeBSD__
	if (iconv(conv, (const char **)&inFake, &in, &outFake, &out) == (size_t)(-1))
#else
	if (iconv(conv, &inFake, &in, &outFake, &out) == (size_t)(-1))
#endif
	{
		delete [] outBuffer;

		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_CODESETCONVERSION, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	dodoString result;
	result.assign(outBuffer, outBefore - out);

	delete [] outBuffer;

	return result;
}

#endif

//-------------------------------------------------------------------

#ifdef ZLIB_EXT

dodoString
tools::zCompress(const dodoString &buffer,
				 unsigned short level,
				 short type)
{
	z_stream strm;
	int ret;

	dodoString strBuf;
	Bytef *byteBuf;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	if ((ret = deflateInit2(&strm, level, Z_DEFLATED, 15, level, type)) < 0)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_ZCOMPRESS, ERR_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);

	strm.avail_in =  buffer.size();
	strm.next_in = (Bytef *)buffer.c_str();

	byteBuf = new Bytef[ZLIB_CHUNK];

	strBuf.clear();

	do
	{
		strm.avail_out = ZLIB_CHUNK;
		strm.next_out = byteBuf;

		if ((ret = deflate(&strm, Z_FINISH)) < 0)
		{
			delete [] byteBuf;

			throw baseEx(ERRMODULE_TOOLS, TOOLSEX_ZCOMPRESS, ERR_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);
		}

		strBuf.append((char *)byteBuf, ZLIB_CHUNK - strm.avail_out);
	}
	while (strm.avail_out == 0);

	deflateEnd(&strm);
	delete [] byteBuf;

	return strBuf;
}

//-------------------------------------------------------------------

dodoString
tools::zDecompress(const dodoString &buffer)
{
	z_stream strm;
	int ret;

	dodoString strBuf;
	Bytef *byteBuf;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	if ((ret = inflateInit2(&strm, 15)) < 0)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_ZDECOMPRESS, ERR_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);

	byteBuf = new Bytef[ZLIB_CHUNK];

	strm.avail_in = buffer.size();
	strm.next_in = (Bytef *)buffer.c_str();

	strBuf.clear();

	do
	{
		strm.avail_out = ZLIB_CHUNK;
		strm.next_out = byteBuf;

		if ((ret = inflate(&strm, Z_NO_FLUSH)) < 0)
		{
			delete [] byteBuf;

			throw baseEx(ERRMODULE_TOOLS, TOOLSEX_ZDECOMPRESS, ERR_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);
		}

		strBuf.append((char *)byteBuf, ZLIB_CHUNK - strm.avail_out);
	}
	while (strm.avail_out == 0);

	inflateEnd(&strm);
	delete [] byteBuf;

	return strBuf;
}

#endif

//-------------------------------------------------------------------

char
tools::hexToChar(const char first,
				 const char second)
{
	int val = 0;

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

			val = (16 * (int (first) - 48));

			break;

		default:

			val = (16 * (int (first) - 55));
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

			val += (int (second) - 48);

			break;

		default:

			val += (int (second) - 55);
	}

	return char (val);
}

//-------------------------------------------------------------------

void
tools::charToHex(char result[3],
				 const char first)
{
	sprintf(result, "%02x", first);
}

//-------------------------------------------------------------------

dodoString
tools::decodeURL(const dodoString &string)
{
	dodoString result;
	unsigned long o(0), k(string.size());

	for (; o < k; ++o)
	{
		switch (string[o])
		{
			case '+':

				result.append(1, ' ');

				break;

			case '%':

				if ((k - o) >= 2 && std::isxdigit(string[o + 1]) && std::isxdigit(string[o + 2]))
				{
					result.append(1, tools::hexToChar(string[o + 1], string[o + 2]));
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

dodoString
tools::encodeURL(const dodoString &string)
{
	dodoString result;

	unsigned long i(0), j(string.size());
	char temp[3];

	for (; i != j; ++i)
	{
		switch (string[i])
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
				tools::charToHex(temp, string[i]);
				result.append(temp);
		}
	}

	return result;
}

//-------------------------------------------------------------------

void
tools::_encodeASCII85(dodoString &result,
					  unsigned long tuple,
					  int count)
{
	short int i(5);
	char buf[5], *s = buf;

	do
	{
		*s++ = tuple % 85;
		tuple /= 85;
	}
	while (--i > 0);

	i = count;
	do
	{
		result.append(1, (char)(*--s + '!'));
	}
	while (i-- > 0);
}

//-------------------------------------------------------------------

dodoString
tools::encodeASCII85(const dodoString &string)
{
	dodoString result("<~");
	unsigned long tuple = 0;

	unsigned short count(0);
	int j = string.size();

	for (int k(0); k < j; ++k)
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
					result.append(1, 'z');
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
tools::_decodeASCII85(dodoString &result,
					  unsigned long tuple,
					  int count)
{
	switch (count)
	{
		case 4:

			result.append(1, (char)(tuple >> 24));
			result.append(1, (char)(tuple >> 16));
			result.append(1, (char)(tuple >>  8));
			result.append(1, (char)(tuple));

			break;

		case 3:

			result.append(1, (char)(tuple >> 24));
			result.append(1, (char)(tuple >> 16));
			result.append(1, (char)(tuple >>  8));

			break;

		case 2:

			result.append(1, (char)(tuple >> 24));
			result.append(1, (char)(tuple >> 16));

			break;

		case 1:

			result.append(1, (char)(tuple >> 24));

			break;
	}
}

//-------------------------------------------------------------------

dodoString
tools::decodeASCII85(const dodoString &string)
{
	unsigned long j = string.size(), count = 0, tuple = 0;
	bool _break = false;
	dodoString result;

	for (unsigned long k(0); k < j; ++k)
	{
		if (string[k] == '<')
		{
			if ((j - k) >= 1 && string[++k] == '~')
			{
				_break = false;
				while (!_break && k < j)
				{
					switch (string[++k])
					{
						case 'z':

							if (count != 0)
								throw baseEx(ERRMODULE_TOOLS, TOOLSEX_DECODEASCII85, ERR_LIBDODO, TOOLSEX_BADASCII85, TOOLSEX_BADASCII85_STR, __LINE__, __FILE__);

							result.append(4, '\0');

							break;

						case '~':

							if ((k - j) >= 1 && string[++k] == '>')
							{
								if (count > 0)
								{
									count--;
									tuple += powASCII85[count];
									_decodeASCII85(result, tuple, count);
								}
								++k;
								_break = true;

								break;
							}

							throw baseEx(ERRMODULE_TOOLS, TOOLSEX_DECODEASCII85, ERR_LIBDODO, TOOLSEX_BADASCII85, TOOLSEX_BADASCII85_STR, __LINE__, __FILE__);

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
								throw baseEx(ERRMODULE_TOOLS, TOOLSEX_DECODEASCII85, ERR_LIBDODO, TOOLSEX_BADASCII85, TOOLSEX_BADASCII85_STR, __LINE__, __FILE__);

							tuple += (string[k] - '!') * powASCII85[count++];
							if (count == 5)
							{
								_decodeASCII85(result, tuple, 4);
								count = 0;
								tuple = 0;
							}
					}
				}
			}
			else
			{
				result.append(1, '<');
				result.append(1, string[k]);
			}
		}
		else
			result.append(1, string[k]);

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
	out[1] = base64EncodeTr[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
	out[2] = (unsigned char)(len > 1 ? base64EncodeTr[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
	out[3] = (unsigned char)(len > 2 ? base64EncodeTr[in[2] & 0x3f ] : '=');
}

//-------------------------------------------------------------------

dodoString
tools::encodeBase64(const dodoString &string)
{
	unsigned long j = string.size();
	unsigned char in[3], out[4];
	unsigned short i, len;
	dodoString result;

	for (unsigned long k(0); k < j;)
	{
		len = 0;
		for (i = 0; i < 3; ++i)
		{
			++k;
			if (k <= j)
			{
				in[i] = string[k - 1];
				++len;
			}
			else
				in[i] = 0;
		}

		if (len > 0)
		{
			_encodeBase64(in, out, len);
			for (i = 0; i < 4; ++i)
				result.append(1, out[i]);
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

dodoString
tools::decodeBase64(const dodoString &string)
{
	unsigned long j = string.size() + 1;
	unsigned char in[4], out[3], v;
	unsigned short i, len;
	dodoString result;

	for (unsigned long k(0); k < j;)
	{
		len = 0;
		for (len = 0, i = 0; i < 4 && k < j; ++i)
		{
			v = 0;
			while (k < j && v == 0)
			{
				v = string[k++];
				v = (unsigned char)((v < 43 || v > 122) ? 0 : base64DecodeTr[v - 43]);
				if (v)
					v = (unsigned char)(v == '$' ? 0 : v - 61);
			}

			if (k < j)
			{
				++len;
				if (v)
					in[i] = (unsigned char)(v - 1);
			}
			else
				in[i] = 0;
		}

		if (len > 0)
		{
			_decodeBase64(in, out);
			for (i = 0; i < len - 1; ++i)
				result.append(1, out[i]);
		}
	}

	return result;
}

//-------------------------------------------------------------------

__url
tools::parseURL(const dodoString &url)
{
	unsigned long begin(0), pos, pos1;

	__url temp;

	if ((pos = url.find("://", 0)) != dodoString::npos)
	{
		temp.protocol = url.substr(0, pos);

		begin = pos + 3;
	}

	if ((pos = url.find('@', begin)) != dodoString::npos)
	{
		if ((pos1 = url.find(':', begin)) < pos)
		{
			temp.login = url.substr(begin, pos1 - begin);

			++pos1;

			temp.password = url.substr(pos1, pos - pos1);
		}
		else
			temp.login = url.substr(begin, pos - begin);

		begin = pos + 1;
	}

	if ((pos = url.find('/', begin)) != dodoString::npos)
	{
		if ((pos1 = url.find(':', begin)) < pos)
		{
			temp.host = url.substr(begin, pos1 - begin);

			++pos1;

			temp.port = url.substr(pos1, pos - pos1);
		}
		else
			temp.host = url.substr(begin, pos - begin);

		begin = pos + 1;

		if ((pos = url.find('?', begin)) != dodoString::npos)
		{
			temp.path = url.substr(begin, pos - begin);
			temp.request = url.substr(pos + 1);
		}
		else
			temp.path = url.substr(begin);
	}
	else
	{
		if ((pos1 = url.find(':', begin)) < pos)
		{
			temp.host = url.substr(begin, pos1 - begin);

			++pos1;

			temp.port = url.substr(pos1, pos - pos1);
		}
		else
			temp.host = url.substr(begin, pos - begin);
	}

	return temp;
}

//-------------------------------------------------------------------

#ifdef BZIP2_EXT

dodoString
tools::bzCompress(const dodoString &buffer,
				  unsigned short level,
				  unsigned short type)
{
	unsigned int len = buffer.size();
	char *dst = new char[len + 1];

	int ret = BZ2_bzBuffToBuffCompress(dst, &len, (char *)buffer.c_str(), len, level, 0, type);
	if (ret != BZ_OK)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_BZCOMPRESS, ERR_BZIP, TOOLSEX_BADBZCOMPRESSION, TOOLSEX_BADBZCOMPRESSION_STR, __LINE__, __FILE__);

	return dodoString(dst, len);
}

//-------------------------------------------------------------------

dodoString
tools::bzDecompress(const dodoString &buffer)
{
	bz_stream bzs;

	bzs.bzalloc = NULL;
	bzs.bzfree = NULL;

	int ret = BZ2_bzDecompressInit(&bzs, 0, 0);
	if (ret != BZ_OK)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_BZDECOMPRESS, ERR_BZIP, TOOLSEX_BADBZDECOMPRESSIONINIT, TOOLSEX_BADBZDECOMPRESSIONINIT_STR, __LINE__, __FILE__);

	int src_len = buffer.size();
	char *src = new char[src_len + 1];
	memcpy(src, buffer.c_str(), src_len);

	unsigned long long size = 0;

	bzs.avail_out = BZIP_CHUNK;
	char *dst = (char *)malloc(bzs.avail_out + 1);
	bzs.next_out = dst;

	bzs.next_in = src;
	bzs.avail_in = src_len;

	dodoString _buffer;

	while ((ret = BZ2_bzDecompress(&bzs)) == BZ_OK && bzs.avail_in > 0)
	{
		bzs.avail_out = src_len;
		size = (bzs.total_out_hi32 * (unsigned int)-1) + bzs.total_out_lo32;
		dst = (char *)realloc(dst, size + bzs.avail_out + 1);
		bzs.next_out = dst + size;
	}

	if (ret == BZ_STREAM_END || ret == BZ_OK)
	{
		size = (bzs.total_out_hi32 * (unsigned int)-1) + bzs.total_out_lo32;
		_buffer.assign(dst, size);

		delete [] src;
		free(dst);
	}
	else
	{
		delete [] src;
		free(dst);

		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_BZDECOMPRESS, ERR_BZIP, TOOLSEX_BADBZDECOMPRESSION, TOOLSEX_BADBZDECOMPRESSION_STR, __LINE__, __FILE__);
	}

	ret = BZ2_bzDecompressEnd(&bzs);
	if (ret != BZ_OK)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_BZDECOMPRESS, ERR_BZIP, TOOLSEX_BADBZDECOMPRESSIONFINISH, TOOLSEX_BADBZDECOMPRESSIONFINISH_STR, __LINE__, __FILE__);

	return _buffer;
}

#endif

//-------------------------------------------------------------------

void
tools::mail(const dodoString &to,
			const dodoString &subject,
			const dodoString &message,
			const dodoString &headers,
			const dodoString &path)
{
	FILE *sendmail = popen((path + " " + to).c_str(), "w");

	if (sendmail == NULL)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	fprintf(sendmail, "To: %s\n", to.c_str());
	fprintf(sendmail, "Subject: %s\n", subject.c_str());
	if (headers.size() > 0)
		fprintf(sendmail, "%s\n", headers.c_str());
	fprintf(sendmail, "\n%s\n", message.c_str());

	if (pclose(sendmail) == -1)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
		x[i] = ((unsigned int)block[j]) | (((unsigned int)block[j + 1]) << 8) | (((unsigned int)block[j + 2]) << 16) | (((unsigned int)block[j + 3]) << 24);

	FF(a, b, c, d, x[ 0], S11, 0xd76aa478);
	FF(d, a, b, c, x[ 1], S12, 0xe8c7b756);
	FF(c, d, a, b, x[ 2], S13, 0x242070db);
	FF(b, c, d, a, x[ 3], S14, 0xc1bdceee);
	FF(a, b, c, d, x[ 4], S11, 0xf57c0faf);
	FF(d, a, b, c, x[ 5], S12, 0x4787c62a);
	FF(c, d, a, b, x[ 6], S13, 0xa8304613);
	FF(b, c, d, a, x[ 7], S14, 0xfd469501);
	FF(a, b, c, d, x[ 8], S11, 0x698098d8);
	FF(d, a, b, c, x[ 9], S12, 0x8b44f7af);
	FF(c, d, a, b, x[10], S13, 0xffff5bb1);
	FF(b, c, d, a, x[11], S14, 0x895cd7be);
	FF(a, b, c, d, x[12], S11, 0x6b901122);
	FF(d, a, b, c, x[13], S12, 0xfd987193);
	FF(c, d, a, b, x[14], S13, 0xa679438e);
	FF(b, c, d, a, x[15], S14, 0x49b40821);

	GG(a, b, c, d, x[ 1], S21, 0xf61e2562);
	GG(d, a, b, c, x[ 6], S22, 0xc040b340);
	GG(c, d, a, b, x[11], S23, 0x265e5a51);
	GG(b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
	GG(a, b, c, d, x[ 5], S21, 0xd62f105d);
	GG(d, a, b, c, x[10], S22,  0x2441453);
	GG(c, d, a, b, x[15], S23, 0xd8a1e681);
	GG(b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
	GG(a, b, c, d, x[ 9], S21, 0x21e1cde6);
	GG(d, a, b, c, x[14], S22, 0xc33707d6);
	GG(c, d, a, b, x[ 3], S23, 0xf4d50d87);
	GG(b, c, d, a, x[ 8], S24, 0x455a14ed);
	GG(a, b, c, d, x[13], S21, 0xa9e3e905);
	GG(d, a, b, c, x[ 2], S22, 0xfcefa3f8);
	GG(c, d, a, b, x[ 7], S23, 0x676f02d9);
	GG(b, c, d, a, x[12], S24, 0x8d2a4c8a);

	HH(a, b, c, d, x[ 5], S31, 0xfffa3942);
	HH(d, a, b, c, x[ 8], S32, 0x8771f681);
	HH(c, d, a, b, x[11], S33, 0x6d9d6122);
	HH(b, c, d, a, x[14], S34, 0xfde5380c);
	HH(a, b, c, d, x[ 1], S31, 0xa4beea44);
	HH(d, a, b, c, x[ 4], S32, 0x4bdecfa9);
	HH(c, d, a, b, x[ 7], S33, 0xf6bb4b60);
	HH(b, c, d, a, x[10], S34, 0xbebfbc70);
	HH(a, b, c, d, x[13], S31, 0x289b7ec6);
	HH(d, a, b, c, x[ 0], S32, 0xeaa127fa);
	HH(c, d, a, b, x[ 3], S33, 0xd4ef3085);
	HH(b, c, d, a, x[ 6], S34,  0x4881d05);
	HH(a, b, c, d, x[ 9], S31, 0xd9d4d039);
	HH(d, a, b, c, x[12], S32, 0xe6db99e5);
	HH(c, d, a, b, x[15], S33, 0x1fa27cf8);
	HH(b, c, d, a, x[ 2], S34, 0xc4ac5665);

	II(a, b, c, d, x[ 0], S41, 0xf4292244);
	II(d, a, b, c, x[ 7], S42, 0x432aff97);
	II(c, d, a, b, x[14], S43, 0xab9423a7);
	II(b, c, d, a, x[ 5], S44, 0xfc93a039);
	II(a, b, c, d, x[12], S41, 0x655b59c3);
	II(d, a, b, c, x[ 3], S42, 0x8f0ccc92);
	II(c, d, a, b, x[10], S43, 0xffeff47d);
	II(b, c, d, a, x[ 1], S44, 0x85845dd1);
	II(a, b, c, d, x[ 8], S41, 0x6fa87e4f);
	II(d, a, b, c, x[15], S42, 0xfe2ce6e0);
	II(c, d, a, b, x[ 6], S43, 0xa3014314);
	II(b, c, d, a, x[13], S44, 0x4e0811a1);
	II(a, b, c, d, x[ 4], S41, 0xf7537e82);
	II(d, a, b, c, x[11], S42, 0xbd3af235);
	II(c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
	II(b, c, d, a, x[ 9], S44, 0xeb86d391);

	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
}

//-------------------------------------------------------------------

void
tools::MD5Final(unsigned char digest[16],
				MD5_CTX       *context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	unsigned int i, j;

	for (i = 0, j = 0; j < 8; ++i, j += 4)
	{
		bits[j] = (unsigned char)(context->count[i] & 0xff);
		bits[j + 1] = (unsigned char)((context->count[i] >> 8) & 0xff);
		bits[j + 2] = (unsigned char)((context->count[i] >> 16) & 0xff);
		bits[j + 3] = (unsigned char)((context->count[i] >> 24) & 0xff);
	}

	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = index < 56 ? 56 - index : 120 - index;
	MD5Update(context, PADDING, padLen);

	MD5Update(context, bits, 8);

	for (i = 0, j = 0; j < 16; ++i, j += 4)
	{
		digest[j] = (unsigned char)(context->state[i] & 0xff);
		digest[j + 1] = (unsigned char)((context->state[i] >> 8) & 0xff);
		digest[j + 2] = (unsigned char)((context->state[i] >> 16) & 0xff);
		digest[j + 3] = (unsigned char)((context->state[i] >> 24) & 0xff);
	}

	memset(context, 0, sizeof(MD5_CTX));
}

//-------------------------------------------------------------------

void
tools::MD5Update(MD5_CTX       *context,
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

dodoString
tools::MD5(const dodoString &string)
{
	MD5_CTX context;
	unsigned char digest[16];

	MD5Init(&context);
	MD5Update(&context, (unsigned char *)string.c_str(), string.size());
	MD5Final(digest, &context);

	return dodoString((char *)digest, 16);
}

//-------------------------------------------------------------------

dodoString
tools::MD5Hex(const dodoString &string)
{
	std::string md5 = MD5(string);
	int j = md5.size();
	std::string md5Hex;
	char tmp[3];

	for (int i = 0; i < j; ++i)
	{
		sprintf(tmp, "%02x", (unsigned char)md5[i]);
		md5Hex.append(tmp);
	}

	return md5Hex;
}

//-------------------------------------------------------------------

void
tools::mail(const dodoString &host,
			int port,
			const dodoString &to,
			const dodoString &from,
			const dodoString &subject,
			const dodoString &message,
			const dodoString &login,
			const dodoString &pass,
			const dodoString &headers)
{
	enum mailAuthTypeEnum
	{
		SMTPAUTH_CRAMMD5 = 2,
		SMTPAUTH_LOGIN = 4,
		SMTPAUTH_PLAIN = 8
	};

	unsigned short authType = 0;

	bool auth = login.size() > 0 ? true : false;
	
	short family = IONETWORKOPTIONS_PROTO_FAMILY_IPV4;
	if (host.find(":") != dodoString::npos)
		family = IONETWORKOPTIONS_PROTO_FAMILY_IPV6;
	
	ioNetworkExchange ex;
	
	ioNetwork net(false, family, IONETWORKOPTIONS_TRANSFER_TYPE_STREAM);
	net.connect(host, port, ex);
	
	dodoString mess;
	
	ex.readStreamString(mess);
	ex.writeStreamString("EHLO " + ioNetworkTools::getLocalName() + "\r\n");
	ex.readStreamString(mess);

	if (stringTools::stringToI(mess.substr(0, 3)) != 250)
		throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_LIBDODO, TOOLSEX_BADMAILHELO, TOOLSEX_BADMAILHELO_STR, __LINE__, __FILE__);

	if (auth)
	{
		if (stringTools::contains(mess, "CRAM-MD5"))
			addFlag(authType, SMTPAUTH_CRAMMD5);

		if (stringTools::contains(mess, "LOGIN"))
			addFlag(authType, SMTPAUTH_LOGIN);

		if (stringTools::contains(mess, "PLAIN"))
			addFlag(authType, SMTPAUTH_PLAIN);
	}

	if (auth)
	{
		if (isSetFlag(authType, SMTPAUTH_CRAMMD5))
		{
			ex.writeStreamString("AUTH CRAM-MD5\r\n");
			ex.readStreamString(mess);

			if (stringTools::stringToI(mess.substr(0, 3)) != 334)
				throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, TOOLSEX_BADMAILAUTH, TOOLSEX_BADMAILAUTH_STR, __LINE__, __FILE__);

			dodoString ticket = decodeBase64(mess.substr(4));

			dodoString md5pass;
			if (pass.size() > 64)
				md5pass = MD5(pass);
			else
				md5pass = pass;

			unsigned char ipad[65];
			unsigned char opad[65];

			memset(ipad, 0, 65);
			memset(opad, 0, 65);
			
			memcpy(ipad, md5pass.c_str(), md5pass.size());
			memcpy(opad, md5pass.c_str(), md5pass.size());

			for (short i = 0; i < 64; ++i)
			{
				ipad[i] ^= 0x36;
				opad[i] ^= 0x5c;
			}

			MD5_CTX context;
			unsigned char digest[16];
			
			MD5Init(&context);
			MD5Update(&context, ipad, 64);
			MD5Update(&context, (unsigned char *)ticket.c_str(), ticket.size());
			MD5Final(digest, &context);

			MD5Init(&context);
			MD5Update(&context, opad, 64);
			MD5Update(&context, digest, 16);
			MD5Final(digest, &context);

			md5pass.clear();
			for (short i = 0; i < 16; ++i)
			{
				sprintf((char *)ipad, "%02x", digest[i]);
				md5pass.append((char *)ipad);
			}

			ex.writeStreamString(encodeBase64(login + " " + md5pass) + "\r\n");
			ex.readStreamString(mess);

			if (stringTools::stringToI(mess.substr(0, 3)) != 235)
				throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, TOOLSEX_BADMAILAUTH, TOOLSEX_BADMAILAUTH_STR, __LINE__, __FILE__);
		}
		else
		{
			if (isSetFlag(authType, SMTPAUTH_LOGIN))
			{
				ex.writeStreamString("AUTH LOGIN\r\n");
				ex.readStreamString(mess);

				if (stringTools::stringToI(mess.substr(0, 3)) != 334)
					throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, TOOLSEX_BADMAILAUTH, TOOLSEX_BADMAILAUTH_STR, __LINE__, __FILE__);

				ex.writeStreamString(encodeBase64(login) + "\r\n");
				ex.readStreamString(mess);
				
				if (stringTools::stringToI(mess.substr(0, 3)) != 334)
					throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, TOOLSEX_BADMAILAUTH, TOOLSEX_BADMAILAUTH_STR, __LINE__, __FILE__);

				ex.writeStreamString(encodeBase64(pass) + "\r\n");
				ex.readStreamString(mess);
				
				if (stringTools::stringToI(mess.substr(0, 3)) != 235)
					throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, TOOLSEX_BADMAILAUTH, TOOLSEX_BADMAILAUTH_STR, __LINE__, __FILE__);
			}
			else
			{
				if (isSetFlag(authType, SMTPAUTH_PLAIN))
				{
					ex.writeStreamString("AUTH PLAIN" + encodeBase64(login + "\0" + login + "\0" + pass) + "\r\n");
					ex.readStreamString(mess);

					if (stringTools::stringToI(mess.substr(0, 3)) != 334)
						throw baseEx(ERRMODULE_TOOLS, TOOLSEX_MAIL, ERR_ERRNO, TOOLSEX_BADMAILAUTH, TOOLSEX_BADMAILAUTH_STR, __LINE__, __FILE__);
				}
			}
		}
	}

	ex.writeStreamString("MAIL FROM: <" + from + ">\r\n");
	ex.readStreamString(mess);

	dodoStringArray pock = explode(to, ",");

	dodoStringArray::iterator i = pock.begin(), j = pock.end();
	for (; i != j; ++i)
	{
		ex.writeStreamString("RCPT TO: <" + *i + ">\r\n");
		ex.readStreamString(mess);
	}

	ex.writeStreamString("DATA\r\n");
	ex.readStreamString(mess);

	ex.writeStreamString("To: " + to + "\r\n");
	ex.writeStreamString("From: " + from + "\r\n");
	ex.writeStreamString("X-Mailer: " PACKAGE_NAME "/" PACKAGE_VERSION "\r\n");
	ex.writeStreamString("Subject: " + subject  + "\r\n");
	ex.writeStreamString(headers);
	ex.writeStreamString(message);
	ex.writeStreamString("\r\n.\r\n");
	ex.writeStreamString("QUIT\r\n");
}

//-------------------------------------------------------------------
