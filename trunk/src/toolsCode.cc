/***************************************************************************
 *            toolsCode.cc
 *
 *  Thu Jul 24 13:29:21 2008
 *  Copyright  2008  Ni@m
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/toolsCode.h>

using namespace dodo::tools;

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
	0x80, 0,	0,        0,	0,        0,		0,    0,		0,    0,		0,    0,		0,    0,		0,    0,		0, 0, 0, 0, 0, 0,
	0,    0,	0,        0,	0,        0,		0,    0,		0,    0,		0,    0,		0,    0,		0,    0,		0, 0, 0, 0, 0, 0,0,
	0,    0,	0,        0,	0,        0,		0,    0,		0,    0,		0,    0,		0,    0,		0,    0,		0, 0, 0
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

/**
 * for SHA-1
 */

/**
 *  Define the SHA1 circular left shift macro
 */
#define SHA1_ROTL(bits,word) \
                (((word) << (bits)) | ((word) >> (32-(bits))))

//-------------------------------------------------------------------\

/**
 * add "length" to the length
 */
#define SHA1AddLength(context, length)						\
    unsigned long addTemp									\
    (addTemp = (context)->Length_Low,						\
     (context)->Corrupted =									\
        (((context)->Length_Low += (length)) < addTemp) &&	\
        (++(context)->Length_High == 0) ? 1 : 0)

//-------------------------------------------------------------------

#ifdef ICONV_EXT

dodoString
code::codesetConversion(const dodoString &buffer,
						const dodoString &toCode,
						const dodoString &fromCode)
{
	iconv_t conv = iconv_open(toCode.c_str(), fromCode.c_str());
	if (conv == (iconv_t)(-1))
		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_CODESETCONVERSION, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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

		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_CODESETCONVERSION, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
code::zCompress(const dodoString &buffer,
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
		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_ZCOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);

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

			throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_ZCOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);
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
code::zDecompress(const dodoString &buffer)
{
	z_stream strm;
	int ret;

	dodoString strBuf;
	Bytef *byteBuf;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	if ((ret = inflateInit2(&strm, 15)) < 0)
		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_ZDECOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);

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

			throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_ZDECOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);
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
code::hexToChar(char first,
				char second)
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

			val = (16 * (int(first) - 48));

			break;

		default:

			if (first > 90)
				first -= 32;

			val = (16 * (int(first) - 55));
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

			val += (int(second) - 48);

			break;

		default:

			if (second > 90)
				second -= 32;

			val += (int(second) - 55);
	}

	return char(val);
}

//-------------------------------------------------------------------

void
code::charToHex(char result[3],
				char first)
{
	unsigned char j;

	j = (first >> 4) & 0xf;
	if (j <= 9)
		result[0] = (j + '0');
	else
		result[0] = (j + 'a' - 10);

	j = first & 0xf;
	if (j <= 9)
		result[1] = (j + '0');
	else
		result[1] = (j + 'a' - 10);

	result[2] = '\0';
}

//-------------------------------------------------------------------

dodoString
code::decodeUrl(const dodoString &string)
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
					result.append(1, code::hexToChar(string[o + 1], string[o + 2]));
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
code::encodeUrl(const dodoString &string)
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
				charToHex(temp, string[i]);
				result.append(temp);
		}
	}

	return result;
}

//-------------------------------------------------------------------

void
code::_encodeASCII85(dodoString &result,
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
code::encodeASCII85(const dodoString &string)
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
					code::_encodeASCII85(result, tuple, count);

				tuple = 0;
				count = 0;

				break;
		}
	}

	if (count > 0)
		code::_encodeASCII85(result, tuple, count);

	result.append("~>");

	return result;
}

//-------------------------------------------------------------------

void
code::_decodeASCII85(dodoString &result,
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
code::decodeASCII85(const dodoString &string)
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
								throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_DECODEASCII85, exception::ERRNO_LIBDODO, CODEEX_BADASCII85, TOOLSCODEEX_BADASCII85_STR, __LINE__, __FILE__);

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

							throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_DECODEASCII85, exception::ERRNO_LIBDODO, CODEEX_BADASCII85, TOOLSCODEEX_BADASCII85_STR, __LINE__, __FILE__);

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
								throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_DECODEASCII85, exception::ERRNO_LIBDODO, CODEEX_BADASCII85, TOOLSCODEEX_BADASCII85_STR, __LINE__, __FILE__);

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
code::_encodeBase64(unsigned char in[3],
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
code::encodeBase64(const dodoString &string)
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

	return result;
}

//-------------------------------------------------------------------

void
code::_decodeBase64(unsigned char in[4],
					unsigned char out[3])
{
	out[0] = (unsigned char )(in[0] << 2 | in[1] >> 4);
	out[1] = (unsigned char )(in[1] << 4 | in[2] >> 2);
	out[2] = (unsigned char )(((in[2] << 6) & 0xc0) | in[3]);
}

//-------------------------------------------------------------------

dodoString
code::decodeBase64(const dodoString &string)
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
code::parseUrl(const dodoString &url)
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

dodoString
code::makeUrl(const __url &url)
{
	dodoString stringUrl;

	unsigned long loginSize = url.login.size();

	if (url.protocol.size() > 0)
	{
		stringUrl.append(url.protocol);
		stringUrl.append("://" );
	}
	if (loginSize > 0)
		stringUrl.append(url.login);
	if (url.password.size() > 0)
	{
		stringUrl.append(":");
		stringUrl.append(url.password);
	}
	if (loginSize > 0)
		stringUrl.append("@");
	stringUrl.append(url.host);
	if (url.port.size() > 0)
	{
		stringUrl.append(":");
		stringUrl.append(url.port);
	}
	stringUrl.append("/");
	stringUrl.append(url.path);
	if (url.request.size() > 0)
	{
		stringUrl.append("?");
		stringUrl.append(url.request);
	}

	return stringUrl;
}

//-------------------------------------------------------------------

#ifdef BZIP2_EXT

dodoString
code::bzCompress(const dodoString &buffer,
				 unsigned short level,
				 unsigned short type)
{
	unsigned int len = buffer.size();
	char *dst = new char[len + 1];

	int ret = BZ2_bzBuffToBuffCompress(dst, &len, (char *)buffer.c_str(), len, level, 0, type);
	if (ret != BZ_OK)
		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_BZCOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZCOMPRESSION, TOOLSCODEEX_BADBZCOMPRESSION_STR, __LINE__, __FILE__);

	return dodoString(dst, len);
}

//-------------------------------------------------------------------

dodoString
code::bzDecompress(const dodoString &buffer)
{
	bz_stream bzs;

	bzs.bzalloc = NULL;
	bzs.bzfree = NULL;

	int ret = BZ2_bzDecompressInit(&bzs, 0, 0);
	if (ret != BZ_OK)
		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_BZDECOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZDECOMPRESSIONINIT, TOOLSCODEEX_BADBZDECOMPRESSIONINIT_STR, __LINE__, __FILE__);

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

		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_BZDECOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZDECOMPRESSION, TOOLSCODEEX_BADBZDECOMPRESSION_STR, __LINE__, __FILE__);
	}

	ret = BZ2_bzDecompressEnd(&bzs);
	if (ret != BZ_OK)
		throw exception::basic(exception::ERRMODULE_TOOLSCODE, CODEEX_BZDECOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZDECOMPRESSIONFINISH, TOOLSCODEEX_BADBZDECOMPRESSIONFINISH_STR, __LINE__, __FILE__);

	return _buffer;
}

#endif

//-------------------------------------------------------------------

void
code::MD5Init(__MD5Context *context)
{
	context->count[0] = context->count[1] = 0;

	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

//-------------------------------------------------------------------

void
code::MD5Transform(unsigned int state[4],
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
code::MD5Final(unsigned char digest[16],
			   __MD5Context       *context)
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

	memset(context, 0, sizeof(__MD5Context));
}

//-------------------------------------------------------------------

void
code::MD5Update(__MD5Context       *context,
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
code::MD5(const dodoString &string)
{
	__MD5Context context;
	unsigned char digest[16];

	MD5Init(&context);
	MD5Update(&context, (unsigned char *)string.c_str(), string.size());
	MD5Final(digest, &context);

	return dodoString((char *)digest, 16);
}

//-------------------------------------------------------------------

dodoString
code::MD5Hex(const dodoString &string)
{
	return binToHex(MD5(string));
}

//-------------------------------------------------------------------

dodoString
code::binToHex(const dodoString &string)
{
	int j = string.size();
	dodoString hex;
	char tmp[3];

	for (int i = 0; i < j; ++i)
	{
		charToHex(tmp, string[i]);
		hex.append(tmp, 2);
	}

	return hex;
}

//-------------------------------------------------------------------

void
code::SHA1Init(__SHA1Context *context)
{
    context->lengthLow = 0;
    context->lengthHigh = 0;
    context->messageBlockIndex = 0;

    /* Initial Hash Values: FIPS-180-2 section 5.3.1 */
    context->intermediateHash[0] = 0x67452301;
    context->intermediateHash[1] = 0xEFCDAB89;
    context->intermediateHash[2] = 0x98BADCFE;
    context->intermediateHash[3] = 0x10325476;
    context->intermediateHash[4] = 0xC3D2E1F0;

    context->computed = 0;
    context->corrupted = 0;
}

//-------------------------------------------------------------------

void
code::SHA1Input(__SHA1Context *context, const unsigned char *bytes, unsigned int bytecount)
{

}

//-------------------------------------------------------------------

void
code::SHA1FinalBits(__SHA1Context *context,
                    const unsigned char bits,
                    unsigned int bitcount)
{

}

//-------------------------------------------------------------------

void
code::SHA1Result(__SHA1Context *context,
                 unsigned char digest[20])
{

}

//-------------------------------------------------------------------

void
code::SHA1Finalize(__SHA1Context *context,
                   unsigned char padByte)
{

}

//-------------------------------------------------------------------

void
code::SHA1PadMessage(__SHA1Context *context,
                     unsigned char padByte)
{

}

//-------------------------------------------------------------------

void
code::SHA1ProcessMessageBlock(__SHA1Context *context)
{

}

//-------------------------------------------------------------------
