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

#include <libdodo/directives.h>

#include <errno.h>
#ifdef ICONV_EXT
#include <iconv.h>
#endif
#ifdef ZLIB_EXT
#include <zlib.h>
#endif
#ifdef BZIP2_EXT
#include <bzlib.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>

#include <libdodo/toolsCode.h>
#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsCodeEx.h>


using namespace dodo::tools;

/**
 * for MD5
 */

/**
 * MD5 magic numbers
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

const unsigned char code::MD5Padding[] = {
	0x80, 0,    0,        0,    0,        0,        0,    0,        0,    0,        0,    0,        0,    0,        0,    0,        0, 0, 0, 0, 0, 0,
	0,    0,    0,        0,    0,        0,        0,    0,        0,    0,        0,    0,        0,    0,        0,    0,        0, 0, 0, 0, 0, 0, 0,
	0,    0,    0,        0,    0,        0,        0,    0,        0,    0,        0,    0,        0,    0,        0,    0,        0, 0, 0
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
const unsigned long code::powASCII85[] = {
	85 * 85 * 85 * 85,
	85 * 85 * 85,
	85 * 85,
	85,
	1
};

//-------------------------------------------------------------------

/**
 * for base64 decode
 */
const char code::base64EncodeChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

//-------------------------------------------------------------------

const char code::base64DecodeChars[] = "|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

//-------------------------------------------------------------------

/**
 * for SHA
 */

#define SHA_Ch(x, y, z) (((x) & ((y) ^ (z))) ^ (z))
#define SHA_Maj(x, y, z) (((x) & ((y) | (z))) | ((y) & (z)))
#define SHA_Parity(x, y, z) ((x) ^ (y) ^ (z))

//-------------------------------------------------------------------

/**
 * for SHA-1
 */

/**
 *  SHA-1 circular left shift macro
 */
#define SHA1_ROTL(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

//-------------------------------------------------------------------

/**
 * for SHA-256
 */

/**
 * SHA-256 rotate left and rotate right macro
 */
#define SHA256_ROTL(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))
#define SHA256_ROTR(bits, word) (((word) >> (bits)) | ((word) << (32 - (bits))))

//-------------------------------------------------------------------

/**
 * SHA-256 SIGMA and sigma macros
 */
#define SHA256_SIGMA0(word) (SHA256_ROTR( 2, word) ^ SHA256_ROTR(13, word) ^ SHA256_ROTR(22, word))
#define SHA256_SIGMA1(word) (SHA256_ROTR( 6, word) ^ SHA256_ROTR(11, word) ^ SHA256_ROTR(25, word))
#define SHA256_sigma0(word) (SHA256_ROTR( 7, word) ^ SHA256_ROTR(18, word) ^ ((word) >> 3))
#define SHA256_sigma1(word) (SHA256_ROTR(17, word) ^ SHA256_ROTR(19, word) ^ ((word) >> 10))

//-------------------------------------------------------------------

/**
 * for SHA-512
 */

/**
 * SHA-512 rotate left and rotate right macro
 */
#define SHA512_ROTL(bits, word) (((word) << (bits)) | ((word) >> (64 - (bits))))
#define SHA512_ROTR(bits, word) (((word) >> (bits)) | ((word) << (64 - (bits))))

/**
 * SHA-512 SIGMA and sigma macros
 */
#define SHA512_SIGMA0(word) (SHA512_ROTR(28, (word)) ^ SHA512_ROTR(34, (word)) ^ SHA512_ROTR(39, (word)))
#define SHA512_SIGMA1(word) (SHA512_ROTR(14, (word)) ^ SHA512_ROTR(18, (word)) ^ SHA512_ROTR(41, (word)))
#define SHA512_sigma0(word) (SHA512_ROTR( 1, (word)) ^ SHA512_ROTR( 8, (word)) ^ ((word) >> 7))
#define SHA512_sigma1(word) (SHA512_ROTR(19, (word)) ^ SHA512_ROTR(61, (word)) ^ ((word) >> 6))

/*
 * These definitions are potentially faster equivalents for the ones
 * used in FIPS-180-2, section 4.1.3.
 *   ((x & y) ^ (~x & z)) becomes
 *   ((x & (y ^ z)) ^ z)
 */
#define SHA512_Ch(x, y, z) ((x & (y ^ z)) ^ z)

//-------------------------------------------------------------------

/*
 * These definitions are potentially faster equivalents for the ones
 * used in FIPS-180-2, section 4.1.3.
 *   ((x & y) ^ (x & z) ^ (y & z)) becomes
 *   ((x & (y | z)) | (y & z))
 */
#define SHA512_Maj(x, y, z) ((x & (y | z)) | (y & z))

//-------------------------------------------------------------------

/**
 * for HEX convertions
 */
const char code::hexEncodeChars[] = "0123456789abcdef";

//-------------------------------------------------------------------

#ifdef ICONV_EXT
dodoString
code::codesetConversion(const dodoString &buffer,
						const dodoString &toCode,
						const dodoString &fromCode)
{
	iconv_t conv = iconv_open(toCode.data(), fromCode.data());
	if (conv == (iconv_t)(-1))
		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_CODESETCONVERSION, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	size_t in, out, outBefore;
	char *inFake, *outFake;

	in = buffer.size();
	outBefore = out = (in + 1) * 4;
	char *outBuffer = new char[out];

	inFake = (char *)buffer.data();
	outFake = outBuffer;

#ifdef __FreeBSD__
	if (iconv(conv, (const char **)&inFake, &in, &outFake, &out) == (size_t)(-1))
#else
	if (iconv(conv, &inFake, &in, &outFake, &out) == (size_t)(-1))
#endif
	{
		delete [] outBuffer;

		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_CODESETCONVERSION, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
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
				unsigned short   level,
				short            type)
{
	z_stream strm;
	int ret;

	dodoString strBuf;
	Bytef *byteBuf;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	if ((ret = deflateInit2(&strm, level, Z_DEFLATED, 15, level, type)) < 0)
		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_ZCOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);

	strm.avail_in =  buffer.size();
	strm.next_in = (Bytef *)buffer.data();

	byteBuf = new Bytef[ZLIB_CHUNK];

	strBuf.clear();

	do {
		strm.avail_out = ZLIB_CHUNK;
		strm.next_out = byteBuf;

		if ((ret = deflate(&strm, Z_FINISH)) < 0) {
			delete [] byteBuf;

			throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_ZCOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);
		}

		strBuf.append((char *)byteBuf, ZLIB_CHUNK - strm.avail_out);
	} while (strm.avail_out == 0);

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
		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_ZDECOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);

	byteBuf = new Bytef[ZLIB_CHUNK];

	strm.avail_in = buffer.size();
	strm.next_in = (Bytef *)buffer.data();

	strBuf.clear();

	do {
		strm.avail_out = ZLIB_CHUNK;
		strm.next_out = byteBuf;

		if ((ret = inflate(&strm, Z_NO_FLUSH)) < 0) {
			delete [] byteBuf;

			throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_ZDECOMPRESS, exception::ERRNO_ZLIB, ret, strm.msg == NULL ? "" : strm.msg, __LINE__, __FILE__);
		}

		strBuf.append((char *)byteBuf, ZLIB_CHUNK - strm.avail_out);
	} while (strm.avail_out == 0);

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

	switch (first) {
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

			val = 16 * (int(first) - 48);

			break;

		default:

			if (first > 90)
				first -= 32;

			val = 16 * int(first) - 55;
	}

	switch (second) {
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

			val += int(second) - 48;

			break;

		default:

			if (second > 90)
				second -= 32;

			val += int(second) - 55;
	}

	return char(val);
}

//-------------------------------------------------------------------

unsigned long
code::hexToLong(const dodoString &string)
{
	long i = string.size() - 1;
	if (i < 0)
		return 0;

	unsigned long val = 0;

	char first = string[i];
	switch (first) {
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

			val = int(first) - 48;

			break;

			case '\0':
				return 0;

		default:

			if (first > 90)
				first -= 32;

			val = int(first) - 55;
	}
	--i;

	for (long j = 1; i >= 0; --i, ++j) {
		first = string[i];
		switch (first) {
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

				val += (2 << ((4 * j) - 1)) * (int(first) - 48);

				break;

			case '\0':
				return val;

			default:

				if (first > 90)
					first -= 32;

				val += (2 << ((4 * j) - 1)) * (int(first) - 55);
		}
	}

	return val;
}

//-------------------------------------------------------------------

void
code::charToHex(char result[3],
				char first)
{
	result[0] = hexEncodeChars[(first >> 4) & 0xf];
	result[1] = hexEncodeChars[first & 0xf];

	result[2] = '\0';
}

//-------------------------------------------------------------------

dodoString
code::longToHex(unsigned long numeric)
{
	dodoString hex;

	for (long i = 2 * sizeof(unsigned long) - 1; i >= 0; --i)
		hex.append(1, hexEncodeChars[(numeric >> i * 4) & 0xf]);

	return hex;
}

//-------------------------------------------------------------------

dodoString
code::decodeUrl(const dodoString &string)
{
	dodoString result;
	unsigned long o(0), k(string.size());

	for (; o < k; ++o) {
		switch (string[o]) {
			case '+':

				result.append(1, ' ');

				break;

			case '%':

				if ((k - o) >= 2 && std::isxdigit(string[o + 1]) && std::isxdigit(string[o + 2])) {
					result.append(1, code::hexToChar(string[o + 1], string[o + 2]));
					o += 2;
				} else
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

	for (; i != j; ++i) {
		switch (string[i]) {
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
code::_encodeASCII85(dodoString    &result,
					 unsigned long tuple,
					 int           count)
{
	short i(5);
	char buf[5], *s = buf;

	do {
		*s++ = tuple % 85;
		tuple /= 85;
	} while (--i > 0);

	i = count;
	do
		result.append(1, (char)(*--s + '!'));
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

	for (int k(0); k < j; ++k) {
		switch (count++) {
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
code::_decodeASCII85(dodoString    &result,
					 unsigned long tuple,
					 int           count)
{
	switch (count) {
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

	for (unsigned long k(0); k < j; ++k) {
		if (string[k] == '<') {
			if ((j - k) >= 1 && string[++k] == '~') {
				_break = false;
				while (!_break && k < j) {
					switch (string[++k]) {
						case 'z':

							if (count != 0)
								throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_DECODEASCII85, exception::ERRNO_LIBDODO, CODEEX_BADASCII85, TOOLSCODEEX_BADASCII85_STR, __LINE__, __FILE__);

							result.append(4, '\0');

							break;

						case '~':

							if ((k - j) >= 1 && string[++k] == '>') {
								if (count > 0) {
									count--;
									tuple += powASCII85[count];
									_decodeASCII85(result, tuple, count);
								}
								++k;
								_break = true;

								break;
							}

							throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_DECODEASCII85, exception::ERRNO_LIBDODO, CODEEX_BADASCII85, TOOLSCODEEX_BADASCII85_STR, __LINE__, __FILE__);

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
								throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_DECODEASCII85, exception::ERRNO_LIBDODO, CODEEX_BADASCII85, TOOLSCODEEX_BADASCII85_STR, __LINE__, __FILE__);

							tuple += (string[k] - '!') * powASCII85[count++];
							if (count == 5) {
								_decodeASCII85(result, tuple, 4);
								count = 0;
								tuple = 0;
							}
					}
				}
			} else {
				result.append(1, '<');
				result.append(1, string[k]);
			}
		} else
			result.append(1, string[k]);
	}

	return result;
}

//-------------------------------------------------------------------

void
code::_encodeBase64(unsigned char in[3],
					unsigned char out[4],
					int           len)
{
	out[0] = base64EncodeChars[in[0] >> 2];
	out[1] = base64EncodeChars[((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4)];
	out[2] = (unsigned char)(len > 1 ? base64EncodeChars[((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
	out[3] = (unsigned char)(len > 2 ? base64EncodeChars[in[2] & 0x3f ] : '=');
}

//-------------------------------------------------------------------

dodoString
code::encodeBase64(const dodoString &string)
{
	unsigned long j = string.size();
	unsigned char in[3], out[4];
	unsigned short i, len;
	dodoString result;

	for (unsigned long k(0); k < j;) {
		len = 0;
		for (i = 0; i < 3; ++i) {
			++k;
			if (k <= j) {
				in[i] = string[k - 1];
				++len;
			} else
				in[i] = 0;
		}

		if (len > 0) {
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

	for (unsigned long k(0); k < j;) {
		len = 0;
		for (len = 0, i = 0; i < 4 && k < j; ++i) {
			v = 0;
			while (k < j && v == 0) {
				v = string[k++];
				v = (unsigned char)((v < 43 || v > 122) ? 0 : base64DecodeChars[v - 43]);
				if (v)
					v = (unsigned char)(v == '$' ? 0 : v - 61);
			}

			if (k < j) {
				++len;
				if (v)
					in[i] = (unsigned char)(v - 1);
			} else
				in[i] = 0;
		}

		if (len > 0) {
			_decodeBase64(in, out);
			for (i = 0; i < len - 1; ++i)
				result.append(1, out[i]);
		}
	}

	return result;
}

//-------------------------------------------------------------------

dodo::__url__
code::parseUrl(const dodoString &url)
{
	unsigned long begin(0), pos, pos1;

	__url__ temp;

	if ((pos = url.find("://", 0)) != dodoString::npos) {
		temp.protocol.assign(url.data(), pos);

		begin = pos + 3;
	}

	if ((pos = url.find('@', begin)) != dodoString::npos) {
		if ((pos1 = url.find(':', begin)) < pos) {
			temp.login.assign(url.data() + begin, pos1 - begin);

			++pos1;

			temp.password.assign(url.data() + pos1, pos - pos1);
		} else
			temp.login.assign(url.data() + begin, pos - begin);

		begin = pos + 1;
	}

	if ((pos = url.find('/', begin)) != dodoString::npos) {
		if ((pos1 = url.find(':', begin)) < pos) {
			temp.host.assign(url.data() + begin, pos1 - begin);

			++pos1;

			temp.port.assign(url.data() + pos1, pos - pos1);
		} else
			temp.host.assign(url.data() + begin, pos - begin);

		begin = pos + 1;

		if ((pos = url.find('?', begin)) != dodoString::npos) {
			temp.path.assign(url.data() + begin, pos - begin);
			temp.request.assign(url.data() + pos + 1, url.size() - pos - 1);
		} else
			temp.path.assign(url.data() + begin, url.size() - begin);
	} else {
		if ((pos1 = url.find(':', begin)) != dodoString::npos) {
			temp.host.assign(url.data() + begin, pos1 - begin);

			++pos1;

			temp.port.assign(url.data() + pos1, url.size() - pos1);
		} else
			temp.host.assign(url.data() + begin, url.size() - begin);
	}

	return temp;
}

//-------------------------------------------------------------------

dodoString
code::makeUrl(const __url__ &url)
{
	dodoString stringUrl;

	unsigned long loginSize = url.login.size();

	if (url.protocol.size() > 0) {
		stringUrl.append(url.protocol);
		stringUrl.append("://" );
	}
	if (loginSize > 0)
		stringUrl.append(url.login);
	if (url.password.size() > 0) {
		stringUrl.append(":");
		stringUrl.append(url.password);
	}
	if (loginSize > 0)
		stringUrl.append("@");
	stringUrl.append(url.host);
	if (url.port.size() > 0) {
		stringUrl.append(":");
		stringUrl.append(url.port);
	}
	stringUrl.append("/");
	stringUrl.append(url.path);
	if (url.request.size() > 0) {
		stringUrl.append("?");
		stringUrl.append(url.request);
	}

	return stringUrl;
}

//-------------------------------------------------------------------

#ifdef BZIP2_EXT
dodoString
code::bzCompress(const dodoString &buffer,
				 unsigned short   level,
				 unsigned short   type)
{
	unsigned int len = buffer.size();
	char *dst = new char[len + 1];

	int ret = BZ2_bzBuffToBuffCompress(dst, &len, (char *)buffer.data(), len, level, 0, type);
	if (ret != BZ_OK)
		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_BZCOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZCOMPRESSION, TOOLSCODEEX_BADBZCOMPRESSION_STR, __LINE__, __FILE__);

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
		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_BZDECOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZDECOMPRESSIONINIT, TOOLSCODEEX_BADBZDECOMPRESSIONINIT_STR, __LINE__, __FILE__);

	int src_len = buffer.size();
	char *src = new char[src_len + 1];
	memcpy(src, buffer.data(), src_len);

	unsigned long long size = 0;

	bzs.avail_out = BZIP_CHUNK;
	char *dst = (char *)malloc(bzs.avail_out + 1);
	bzs.next_out = dst;

	bzs.next_in = src;
	bzs.avail_in = src_len;

	dodoString _buffer;

	while ((ret = BZ2_bzDecompress(&bzs)) == BZ_OK && bzs.avail_in > 0) {
		bzs.avail_out = src_len;
		size = (bzs.total_out_hi32 * (unsigned int)-1) + bzs.total_out_lo32;
		dst = (char *)realloc(dst, size + bzs.avail_out + 1);
		bzs.next_out = dst + size;
	}

	if (ret == BZ_STREAM_END || ret == BZ_OK) {
		size = (bzs.total_out_hi32 * (unsigned int)-1) + bzs.total_out_lo32;
		_buffer.assign(dst, size);

		delete [] src;
		free(dst);
	} else {
		delete [] src;
		free(dst);

		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_BZDECOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZDECOMPRESSION, TOOLSCODEEX_BADBZDECOMPRESSION_STR, __LINE__, __FILE__);
	}

	ret = BZ2_bzDecompressEnd(&bzs);
	if (ret != BZ_OK)
		throw exception::basic(exception::MODULE_TOOLSCODE, CODEEX_BZDECOMPRESS, exception::ERRNO_BZIP, CODEEX_BADBZDECOMPRESSIONFINISH, TOOLSCODEEX_BADBZDECOMPRESSIONFINISH_STR, __LINE__, __FILE__);

	return _buffer;
}
#endif

//-------------------------------------------------------------------

void
code::MD5Init(__MD5Context__ *context)
{
	context->count[0] = context->count[1] = 0;

	context->state[0] = 0x67452301;
	context->state[1] = 0xefcdab89;
	context->state[2] = 0x98badcfe;
	context->state[3] = 0x10325476;
}

//-------------------------------------------------------------------

void
code::MD5Transform(unsigned int        state[4],
				   const unsigned char block[64])
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
code::MD5Final(unsigned char  digest[16],
			   __MD5Context__ *context)
{
	unsigned char bits[8];
	unsigned int index, padLen;

	unsigned int i, j;

	for (i = 0, j = 0; j < 8; ++i, j += 4) {
		bits[j] = (unsigned char)(context->count[i] & 0xff);
		bits[j + 1] = (unsigned char)((context->count[i] >> 8) & 0xff);
		bits[j + 2] = (unsigned char)((context->count[i] >> 16) & 0xff);
		bits[j + 3] = (unsigned char)((context->count[i] >> 24) & 0xff);
	}

	index = (unsigned int)((context->count[0] >> 3) & 0x3f);
	padLen = index < 56 ? 56 - index : 120 - index;
	MD5Update(context, MD5Padding, padLen);

	MD5Update(context, bits, 8);

	for (i = 0, j = 0; j < 16; ++i, j += 4) {
		digest[j] = (unsigned char)(context->state[i] & 0xff);
		digest[j + 1] = (unsigned char)((context->state[i] >> 8) & 0xff);
		digest[j + 2] = (unsigned char)((context->state[i] >> 16) & 0xff);
		digest[j + 3] = (unsigned char)((context->state[i] >> 24) & 0xff);
	}

	memset(context, 0, sizeof(__MD5Context__));
}

//-------------------------------------------------------------------

void
code::MD5Update(__MD5Context__      *context,
				const unsigned char *input,
				unsigned int        inputLen)
{
	unsigned int i, index, partLen;

	index = (unsigned int )((context->count[0] >> 3) & 0x3F);

	if ((context->count[0] += (inputLen << 3)) < (inputLen << 3))
		++context->count[1];

	context->count[1] += (inputLen >> 29);

	partLen = 64 - index;

	if (inputLen >= partLen) {
		memcpy(&context->buffer[index], input, partLen);

		MD5Transform(context->state, context->buffer);

		for (i = partLen; i + 63 < inputLen; i += 64)
			MD5Transform(context->state, &input[i]);

		index = 0;
	} else
		i = 0;

	memcpy(&context->buffer[index], &input[i], inputLen - i);
}

//-------------------------------------------------------------------

dodoString
code::MD5(const dodoString &string)
{
	__MD5Context__ context;
	unsigned char digest[16];

	MD5Init(&context);
	MD5Update(&context, (unsigned char *)string.data(), string.size());
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

	for (int i = 0; i < j; ++i) {
		charToHex(tmp, string[i]);
		hex.append(tmp, 2);
	}

	return hex;
}

//-------------------------------------------------------------------

void
code::SHA1Init(__SHA1_256Context__ *context)
{
	context->length = 0;
	context->messageBlockIndex = 0;

		/* Initial Hash Values: FIPS-180-2 section 5.3.1 */
	context->intermediateHash[0] = 0x67452301;
	context->intermediateHash[1] = 0xEFCDAB89;
	context->intermediateHash[2] = 0x98BADCFE;
	context->intermediateHash[3] = 0x10325476;
	context->intermediateHash[4] = 0xC3D2E1F0;
}

//-------------------------------------------------------------------

void
code::SHA1Input(__SHA1_256Context__ *context,
				const unsigned char *bytes,
				unsigned int        bytecount)
{
	if (bytecount == 0)
		return;

	while (bytecount--) {
		context->messageBlock[context->messageBlockIndex++] = (*bytes & 0xFF);

		context->length += 8;
		if (context->messageBlockIndex == 64)
			SHA1ProcessMessageBlock(context);

		++bytes;
	}
}

//-------------------------------------------------------------------

void
code::SHA1Result(__SHA1_256Context__ *context,
				 unsigned char       digest[20])
{
	SHA1PadMessage(context, 0x80);

	for (int i = 0; i < 20; ++i)
		digest[i] = context->intermediateHash[i >> 2] >> 8 * ( 3 - ( i & 0x03 ));
}

//-------------------------------------------------------------------

void
code::SHA1PadMessage(__SHA1_256Context__ *context,
					 unsigned char       padByte)
{
	if (context->messageBlockIndex >= 56) {
		context->messageBlock[context->messageBlockIndex++] = padByte;

		while (context->messageBlockIndex < 64)
			context->messageBlock[context->messageBlockIndex++] = 0;

		SHA1ProcessMessageBlock(context);
	} else
		context->messageBlock[context->messageBlockIndex++] = padByte;

	while (context->messageBlockIndex < 56)
		context->messageBlock[context->messageBlockIndex++] = 0;

	context->messageBlock[56] = context->length >> 56;
	context->messageBlock[57] = context->length >> 48;
	context->messageBlock[58] = context->length >> 40;
	context->messageBlock[59] = context->length >> 32;
	context->messageBlock[60] = context->length >> 24;
	context->messageBlock[61] = context->length >> 16;
	context->messageBlock[62] = context->length >> 8;
	context->messageBlock[63] = context->length;

	SHA1ProcessMessageBlock(context);
}

//-------------------------------------------------------------------

void
code::SHA1ProcessMessageBlock(__SHA1_256Context__ *context)
{
		/* Constants defined in FIPS-180-2, section 4.2.1 */
	const static unsigned long K[4] = {
		0x5A827999, 0x6ED9EBA1, 0x8F1BBCDC, 0xCA62C1D6
	};

	int t;
	unsigned long temp;
	unsigned long W[80];
	unsigned long A, B, C, D, E;

	for (t = 0; t < 16; ++t) {
		W[t]  = (unsigned long)context->messageBlock[t * 4] << 24;
		W[t] |= (unsigned long)context->messageBlock[t * 4 + 1] << 16;
		W[t] |= (unsigned long)context->messageBlock[t * 4 + 2] << 8;
		W[t] |= (unsigned long)context->messageBlock[t * 4 + 3];
	}

	for (t = 16; t < 80; ++t)
		W[t] = SHA1_ROTL(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);

	A = context->intermediateHash[0];
	B = context->intermediateHash[1];
	C = context->intermediateHash[2];
	D = context->intermediateHash[3];
	E = context->intermediateHash[4];

	for (t = 0; t < 20; ++t) {
		temp = SHA1_ROTL(5, A) + SHA_Ch(B, C, D) + E + W[t] + K[0];
		E = D;
		D = C;
		C = SHA1_ROTL(30, B);
		B = A;
		A = temp;
	}

	for (t = 20; t < 40; ++t) {
		temp = SHA1_ROTL(5, A) + SHA_Parity(B, C, D) + E + W[t] + K[1];
		E = D;
		D = C;
		C = SHA1_ROTL(30, B);
		B = A;
		A = temp;
	}

	for (t = 40; t < 60; ++t) {
		temp = SHA1_ROTL(5, A) + SHA_Maj(B, C, D) + E + W[t] + K[2];
		E = D;
		D = C;
		C = SHA1_ROTL(30, B);
		B = A;
		A = temp;
	}

	for (t = 60; t < 80; ++t) {
		temp = SHA1_ROTL(5, A) + SHA_Parity(B, C, D) + E + W[t] + K[3];
		E = D;
		D = C;
		C = SHA1_ROTL(30, B);
		B = A;
		A = temp;
	}

	context->intermediateHash[0] += A;
	context->intermediateHash[1] += B;
	context->intermediateHash[2] += C;

	context->intermediateHash[3] += D;
	context->intermediateHash[4] += E;

	context->messageBlockIndex = 0;
}

//-------------------------------------------------------------------

dodoString
code::SHA1(const dodoString &string)
{
	__SHA1_256Context__ context;
	unsigned char digest[20];

	SHA1Init(&context);
	SHA1Input(&context, (unsigned char *)string.data(), string.size());
	SHA1Result(&context, digest);

	return dodoString((char *)digest, 20);
}

//-------------------------------------------------------------------

dodoString
code::SHA1Hex(const dodoString &string)
{
	return binToHex(SHA1(string));
}

//-------------------------------------------------------------------

void
code::SHA256Init(__SHA1_256Context__ *context)
{
	context->length = 0;
	context->messageBlockIndex = 0;

		/* Initial Hash Values: FIPS-180-2 section 5.3.2 */
	context->intermediateHash[0] = 0x6A09E667;
	context->intermediateHash[1] = 0xBB67AE85;
	context->intermediateHash[2] = 0x3C6EF372;
	context->intermediateHash[3] = 0xA54FF53A;
	context->intermediateHash[4] = 0x510E527F;
	context->intermediateHash[5] = 0x9B05688C;
	context->intermediateHash[6] = 0x1F83D9AB;
	context->intermediateHash[7] = 0x5BE0CD19;
}

//-------------------------------------------------------------------

void
code::SHA256Input(__SHA1_256Context__ *context,
				  const unsigned char *bytes,
				  unsigned int        bytecount)
{
	if (bytecount == 0)
		return;

	while (bytecount--) {
		context->messageBlock[context->messageBlockIndex++] = (*bytes & 0xFF);

		context->length += 8;
		if (context->messageBlockIndex == 64)
			SHA256ProcessMessageBlock(context);

		bytes++;
	}
}

//-------------------------------------------------------------------

void
code::SHA256Result(__SHA1_256Context__ *context,
				   unsigned char       digest[32])
{
	SHA256PadMessage(context, 0x80);

	for (int i = 0; i < 32; ++i)
		digest[i] = (unsigned char)(context->intermediateHash[i >> 2] >> 8 * ( 3 - ( i & 0x03 )));
}

//-------------------------------------------------------------------

void
code::SHA256PadMessage(__SHA1_256Context__ *context,
					   unsigned char       padByte)
{
	if (context->messageBlockIndex >= 56) {
		context->messageBlock[context->messageBlockIndex++] = padByte;

		while (context->messageBlockIndex < 64)
			context->messageBlock[context->messageBlockIndex++] = 0;

		SHA256ProcessMessageBlock(context);
	} else
		context->messageBlock[context->messageBlockIndex++] = padByte;

	while (context->messageBlockIndex < 56)
		context->messageBlock[context->messageBlockIndex++] = 0;

	context->messageBlock[56] = context->length >> 56;
	context->messageBlock[57] = context->length >> 48;
	context->messageBlock[58] = context->length >> 40;
	context->messageBlock[59] = context->length >> 32;
	context->messageBlock[60] = context->length >> 24;
	context->messageBlock[61] = context->length >> 16;
	context->messageBlock[62] = context->length >> 8;
	context->messageBlock[63] = context->length;

	SHA256ProcessMessageBlock(context);
}

//-------------------------------------------------------------------

void
code::SHA256ProcessMessageBlock(__SHA1_256Context__ *context)
{
		/* Constants defined in FIPS-180-2, section 4.2.2 */
	static const uint32_t K[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b,
		0x59f111f1, 0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01,
		0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7,
		0xc19bf174, 0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
		0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da, 0x983e5152,
		0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
		0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc,
		0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819,
		0xd6990624, 0xf40e3585, 0x106aa070, 0x19a4c116, 0x1e376c08,
		0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f,
		0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	int t, t4;
	unsigned long temp1, temp2;
	unsigned long W[64];
	unsigned long A, B, C, D, E, F, G, H;

	/*
	 * Initialize the first 16 words in the array W
	 */
	for (t = t4 = 0; t < 16; t++, t4 += 4) {
		W[t] = ((unsigned long)context->messageBlock[t4] << 24) |
			   ((unsigned long)context->messageBlock[t4 + 1] << 16) |
			   ((unsigned long)context->messageBlock[t4 + 2] << 8) |
			   ((unsigned long)context->messageBlock[t4 + 3]);
	}

	for (t = 16; t < 64; t++)
		W[t] = SHA256_sigma1(W[t - 2]) + W[t - 7] + SHA256_sigma0(W[t - 15]) + W[t - 16];

	A = context->intermediateHash[0];
	B = context->intermediateHash[1];
	C = context->intermediateHash[2];
	D = context->intermediateHash[3];
	E = context->intermediateHash[4];
	F = context->intermediateHash[5];
	G = context->intermediateHash[6];
	H = context->intermediateHash[7];

	for (t = 0; t < 64; t++) {
		temp1 = H + SHA256_SIGMA1(E) + SHA_Ch(E, F, G) + K[t] + W[t];
		temp2 = SHA256_SIGMA0(A) + SHA_Maj(A, B, C);
		H = G;
		G = F;
		F = E;
		E = D + temp1;
		D = C;
		C = B;
		B = A;
		A = temp1 + temp2;
	}

	context->intermediateHash[0] += A;
	context->intermediateHash[1] += B;
	context->intermediateHash[2] += C;
	context->intermediateHash[3] += D;
	context->intermediateHash[4] += E;
	context->intermediateHash[5] += F;
	context->intermediateHash[6] += G;
	context->intermediateHash[7] += H;

	context->messageBlockIndex = 0;
}

//-------------------------------------------------------------------

dodoString
code::SHA256(const dodoString &string)
{
	__SHA1_256Context__ context;
	unsigned char digest[32];

	SHA256Init(&context);
	SHA256Input(&context, (unsigned char *)string.data(), string.size());
	SHA256Result(&context, digest);

	return dodoString((char *)digest, 32);
}

//-------------------------------------------------------------------

dodoString
code::SHA256Hex(const dodoString &string)
{
	return binToHex(SHA256(string));
}

//-------------------------------------------------------------------

void
code::SHA512Init(__SHA512Context__ *context)
{
	context->messageBlockIndex = 0;

	context->lengthLow = context->lengthHigh = 0;

	context->intermediateHash[0] = 0x6A09E667F3BCC908ULL;
	context->intermediateHash[1] = 0xBB67AE8584CAA73BULL;
	context->intermediateHash[2] = 0x3C6EF372FE94F82BULL;
	context->intermediateHash[3] = 0xA54FF53A5F1D36F1ULL;
	context->intermediateHash[4] = 0x510E527FADE682D1ULL;
	context->intermediateHash[5] = 0x9B05688C2B3E6C1FULL;
	context->intermediateHash[6] = 0x1F83D9ABFB41BD6BULL;
	context->intermediateHash[7] = 0x5BE0CD19137E2179ULL;

	context->corrupted = false;
}

//-------------------------------------------------------------------

void
code::SHA512Input(__SHA512Context__   *context,
				  const unsigned char *bytes,
				  unsigned int        bytecount)
{
	if (bytecount == 0)
		return;

	unsigned long addTemp;
#define SHA512AddLength(context, length) (addTemp = (context)->lengthLow, (context)->corrupted = (((context)->lengthLow += (length)) < addTemp) && (++(context)->lengthHigh == 0))

	while (bytecount-- && !context->corrupted) {
		context->messageBlock[context->messageBlockIndex++] = (*bytes & 0xFF);

		SHA512AddLength(context, 8);
		if (!context->corrupted && (context->messageBlockIndex == 128))
			SHA512ProcessMessageBlock(context);

		bytes++;
	}

#undef SHA512AddLength
}

//-------------------------------------------------------------------

void
code::SHA512Result(__SHA512Context__ *context,
				   unsigned char     digest[64])
{
	SHA512PadMessage(context, 0x80);

	for (int i = -1, j = 0; i < 63; ++j) {
		digest[++i] = context->intermediateHash[j] >> 56;
		digest[++i] = context->intermediateHash[j] >> 48;
		digest[++i] = context->intermediateHash[j] >> 40;
		digest[++i] = context->intermediateHash[j] >> 32;
		digest[++i] = context->intermediateHash[j] >> 24;
		digest[++i] = context->intermediateHash[j] >> 16;
		digest[++i] = context->intermediateHash[j] >> 8;
		digest[++i] = context->intermediateHash[j];
	}
}

//-------------------------------------------------------------------

void
code::SHA512PadMessage(__SHA512Context__ *context,
					   unsigned char     padByte)
{
	if (context->messageBlockIndex >= 112) {
		context->messageBlock[context->messageBlockIndex++] = padByte;

		while (context->messageBlockIndex < 128)
			context->messageBlock[context->messageBlockIndex++] = 0;

		SHA512ProcessMessageBlock(context);
	} else
		context->messageBlock[context->messageBlockIndex++] = padByte;

	while (context->messageBlockIndex < 112)
		context->messageBlock[context->messageBlockIndex++] = 0;

	context->messageBlock[112] = context->lengthHigh >> 56;
	context->messageBlock[113] = context->lengthHigh >> 48;
	context->messageBlock[114] = context->lengthHigh >> 40;
	context->messageBlock[115] = context->lengthHigh >> 32;
	context->messageBlock[116] = context->lengthHigh >> 24;
	context->messageBlock[117] = context->lengthHigh >> 16;
	context->messageBlock[118] = context->lengthHigh >> 8;
	context->messageBlock[119] = context->lengthHigh;

	context->messageBlock[120] = context->lengthLow >> 56;
	context->messageBlock[121] = context->lengthLow >> 48;
	context->messageBlock[122] = context->lengthLow >> 40;
	context->messageBlock[123] = context->lengthLow >> 32;
	context->messageBlock[124] = context->lengthLow >> 24;
	context->messageBlock[125] = context->lengthLow >> 16;
	context->messageBlock[126] = context->lengthLow >> 8;
	context->messageBlock[127] = context->lengthLow;

	SHA512ProcessMessageBlock(context);
}

//-------------------------------------------------------------------

void
code::SHA512ProcessMessageBlock(__SHA512Context__ *context)
{
		/* Constants defined in FIPS-180-2, section 4.2.3 */
	const unsigned long long K[80] = {
		0x428A2F98D728AE22ULL, 0x7137449123EF65CDULL, 0xB5C0FBCFEC4D3B2FULL, 0xE9B5DBA58189DBBCULL,
		0x3956C25BF348B538ULL, 0x59F111F1B605D019ULL, 0x923F82A4AF194F9BULL, 0xAB1C5ED5DA6D8118ULL,
		0xD807AA98A3030242ULL, 0x12835B0145706FBEULL, 0x243185BE4EE4B28CULL, 0x550C7DC3D5FFB4E2ULL,
		0x72BE5D74F27B896FULL, 0x80DEB1FE3B1696B1ULL, 0x9BDC06A725C71235ULL, 0xC19BF174CF692694ULL,
		0xE49B69C19EF14AD2ULL, 0xEFBE4786384F25E3ULL, 0x0FC19DC68B8CD5B5ULL, 0x240CA1CC77AC9C65ULL,
		0x2DE92C6F592B0275ULL, 0x4A7484AA6EA6E483ULL, 0x5CB0A9DCBD41FBD4ULL, 0x76F988DA831153B5ULL,
		0x983E5152EE66DFABULL, 0xA831C66D2DB43210ULL, 0xB00327C898FB213FULL, 0xBF597FC7BEEF0EE4ULL,
		0xC6E00BF33DA88FC2ULL, 0xD5A79147930AA725ULL, 0x06CA6351E003826FULL, 0x142929670A0E6E70ULL,
		0x27B70A8546D22FFCULL, 0x2E1B21385C26C926ULL, 0x4D2C6DFC5AC42AEDULL, 0x53380D139D95B3DFULL,
		0x650A73548BAF63DEULL, 0x766A0ABB3C77B2A8ULL, 0x81C2C92E47EDAEE6ULL, 0x92722C851482353BULL,
		0xA2BFE8A14CF10364ULL, 0xA81A664BBC423001ULL, 0xC24B8B70D0F89791ULL, 0xC76C51A30654BE30ULL,
		0xD192E819D6EF5218ULL, 0xD69906245565A910ULL, 0xF40E35855771202AULL, 0x106AA07032BBD1B8ULL,
		0x19A4C116B8D2D0C8ULL, 0x1E376C085141AB53ULL, 0x2748774CDF8EEB99ULL, 0x34B0BCB5E19B48A8ULL,
		0x391C0CB3C5C95A63ULL, 0x4ED8AA4AE3418ACBULL, 0x5B9CCA4F7763E373ULL, 0x682E6FF3D6B2B8A3ULL,
		0x748F82EE5DEFB2FCULL, 0x78A5636F43172F60ULL, 0x84C87814A1F0AB72ULL, 0x8CC702081A6439ECULL,
		0x90BEFFFA23631E28ULL, 0xA4506CEBDE82BDE9ULL, 0xBEF9A3F7B2C67915ULL, 0xC67178F2E372532BULL,
		0xCA273ECEEA26619CULL, 0xD186B8C721C0C207ULL, 0xEADA7DD6CDE0EB1EULL, 0xF57D4F7FEE6ED178ULL,
		0x06F067AA72176FBAULL, 0x0A637DC5A2C898A6ULL, 0x113F9804BEF90DAEULL, 0x1B710B35131C471BULL,
		0x28DB77F523047D84ULL, 0x32CAAB7B40C72493ULL, 0x3C9EBE0A15C9BEBCULL, 0x431D67C49C100D4CULL,
		0x4CC5D4BECB3E42B6ULL, 0x597F299CFC657E2AULL, 0x5FCB6FAB3AD6FAECULL, 0x6C44198C4A475817ULL
	};

	int t, t8;
	unsigned long long temp1, temp2;
	unsigned long long W[80];
	unsigned long long A, B, C, D, E, F, G, H;

	for (t = t8 = 0; t < 16; t8 += 8, ++t) {
		W[t] = ((unsigned long long)context->messageBlock[t8] << 56) |
			   ((unsigned long long)context->messageBlock[t8 + 1] << 48) |
			   ((unsigned long long)context->messageBlock[t8 + 2] << 40) |
			   ((unsigned long long)context->messageBlock[t8 + 3] << 32) |
			   ((unsigned long long)context->messageBlock[t8 + 4] << 24) |
			   ((unsigned long long)context->messageBlock[t8 + 5] << 16) |
			   ((unsigned long long)context->messageBlock[t8 + 6] <<  8) |
			   ((unsigned long long)context->messageBlock[t8 + 7]);
	}

	for (t = 16; t < 80; ++t)
		W[t] = SHA512_sigma1(W[t - 2]) + W[t - 7] + SHA512_sigma0(W[t - 15]) + W[t - 16];

	A = context->intermediateHash[0];
	B = context->intermediateHash[1];
	C = context->intermediateHash[2];
	D = context->intermediateHash[3];
	E = context->intermediateHash[4];
	F = context->intermediateHash[5];
	G = context->intermediateHash[6];
	H = context->intermediateHash[7];

	for (t = 0; t < 80; ++t) {
		temp1 = H + SHA512_SIGMA1(E) + SHA512_Ch(E, F, G) + K[t] + W[t];

		temp2 = SHA512_SIGMA0(A) + SHA512_Maj(A, B, C);

		H = G;
		G = F;
		F = E;

		E = temp1 + D;

		D = C;
		C = B;
		B = A;

		A = temp1 + temp2;
	}

	context->intermediateHash[0] += A;
	context->intermediateHash[1] += B;
	context->intermediateHash[2] += C;
	context->intermediateHash[3] += D;
	context->intermediateHash[4] += E;
	context->intermediateHash[5] += F;
	context->intermediateHash[6] += G;
	context->intermediateHash[7] += H;

	context->messageBlockIndex = 0;
}

//-------------------------------------------------------------------

dodoString
code::SHA512(const dodoString &string)
{
	__SHA512Context__ context;
	unsigned char digest[64];

	SHA512Init(&context);
	SHA512Input(&context, (unsigned char *)string.data(), string.size());
	SHA512Result(&context, digest);

	return dodoString((char *)digest, 64);
}

//-------------------------------------------------------------------

dodoString
code::SHA512Hex(const dodoString &string)
{
	return binToHex(SHA512(string));
}

//-------------------------------------------------------------------
