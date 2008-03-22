/***************************************************************************
 *            tools.h
 *
 *  Wed Aug 24 00:42:51 2005
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

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <libdodo/directives.h>

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
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <libdodo/ioSocketOptions.h>
#include <libdodo/toolsEx.h>
#include <libdodo/types.h>

namespace dodo
{
	/**
	 * @def addFlag add bit flag to the statement
	 */
#define addFlag(statement, flag)       (statement) |= (flag)

	/**
	 * @def removeFlag remove bit flag from the statement
	 */
#define removeFlag(statement, flag)    (statement) &= (~(flag))

	/**
	 * @def isSetFlag return true if bit flag is set
	 */
#define isSetFlag(statement, flag)     ((statement) & (flag)) != 0

	/**
	 * @typedef escape defines escape function for explode/implode
	 */
	typedef dodoString (*escape)(const dodoString &);

#ifdef ZLIB_EXT

	/**
	 * @enum toolsZlibCompressionStrategyEnum defines type of compression for zlib
	 */
	enum toolsZlibCompressionStrategyEnum
	{
		TOOLS_ZLIB_FILTRED_COMRESSION = 1,
		TOOLS_ZLIB_HUFFMAN_COMRESSION,
		TOOLS_ZLIB_RLE_COMRESSION,
		TOOLS_ZLIB_FIXED_COMRESSION
	};

#endif

	/**
	 * @enum toolsRandomStrengthEnum defines type of randomness
	 */
	enum toolsRandomStrengthEnum
	{
		TOOLS_RANDOMSTRENGTH_DEFAULT,
		TOOLS_RANDOMSTRENGTH_STRONG
	};

	/**
	 * @struct __url defines URL parts
	 */
	struct __url
	{
		dodoString protocol; ///< scheme
		dodoString host; ///< host name
		dodoString path; ///< path to CGI
		dodoString request; ///< CGI request
		dodoString login; ///< login
		dodoString password; ///< password
		dodoString port; ///< port
	};

	/**
	 * @class tools provides misc functionality
	 */
	class tools
	{
		public:

			/**
			 * generate random data
			 * @param data defines pointer to store data
			 * @param size defines size of random data in bytes
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static void random(void *data, unsigned long size, short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random data
			 * @param size defines size of random data in bytes
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 * '\0' is replaced with '*'
			 */
			static dodoString stringRandom(unsigned long size, short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random unsigned long
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static unsigned long ulRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random long
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static long lRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random unsigned int
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static unsigned int uiRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random int
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static int iRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random unsigned short
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static unsigned short usRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random short
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static short sRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random unsigned char
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static unsigned char ucRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random char
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static char cRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return random double
			 * @param strength defines the ramndomness
			 * @note based on /dev/(u)?random
			 * TOOLS_RANDOMSTRENGTH_STRONG may block
			 */
			static double dRandom(short strength = TOOLS_RANDOMSTRENGTH_DEFAULT);

			/**
			 * @return true if needle has been found
			 * @param arr defines array to search in
			 * @param needle defines string to search for
			 * @param icase defines case sensitivity 
			 */
			static bool isInArray(const dodoStringArray &arr, const dodoString &needle, bool icase);

			/**
			 * @return array of substrings
			 * @param string defines string to explode 
			 * @param separator defines separator string
			 * @param limit defines maxim number of parts to divide string; -1 for unlimit
			 */
			static dodoStringArray explode(const dodoString &string, const dodoString &separator, int limit = -1);

			/**
			 * @return array of substrings
			 * @param string defines string to explode
			 * @param escapeF defines function which will escape every substring 
			 * @param separator defines separator string
			 * @param limit defines maxim number of parts to divide string; -1 for unlimit
			 */
			static dodoStringArray explode(const dodoString &string, escape escapeF, const dodoString &separator, int limit = -1);

			/**
			 * @return string of substrings, separated with separator
			 * @param fields defines substrings to implode
			 * @param escapeF defines function which will escape every substring  
			 * @param separator defines separator string
			 * @param frame defines frame string[frame='; substring => 'substring']
			 * @param limit defines maxim number of parts to divide string; -1 for unlimit
			 */
			static dodoString implode(const dodoStringArray &fields, escape escapeF, const dodoString &separator, const dodoString &frame, int limit = -1);

			/**
			 * @return string of substrings, separated with separator
			 * @param fields defines substrings to implode
			 * @param escapeF defines function which will escape every substring  
			 * @param separator defines separator string
			 * @param limit defines maxim number of parts to divide string; -1 for unlimit
			 */
			static dodoString implode(const dodoStringArray &fields, escape escapeF, const dodoString &separator, int limit = -1);

			/**
			 * @return string of substrings, separated with separator
			 * @param fields defines substrings to implode
			 * @param separator defines separator string
			 * @param limit defines maxim number of parts to divide string; -1 for unlimit
			 */
			static dodoString implode(const dodoStringArray &fields, const dodoString &separator, int limit = -1);

			/**
			 * @return string of substrings, separated with separator
			 * @param fields defines substrings to implode
			 * @param separator defines separator string
			 * @param frame defines frame string[frame='; substring => 'substring']
			 * @param limit defines maxim number of parts to divide string; -1 for unlimit
			 */
			static dodoString implode(const dodoStringArray &fields, const dodoString &separator, const dodoString &frame, int limit = -1);

#ifdef ICONV_EXT

			/**
			 * convert from one codeset to another
			 * @return converted string
			 * @param buffer defines string to convert
			 * @param toCode defines codeset in what perform conversion
			 * @param fromCode defines buffer codeset
			 */
			static dodoString codesetConversion(const dodoString &buffer, const dodoString &toCode, const dodoString &fromCode);

#endif

#ifdef ZLIB_EXT

			/**
			 * @return compressed buffer
			 * @param buffer defines data to compress
			 * @param level defines compression level[1..9]
			 * @param type defines compression strategy[see toolsZlibCompressionStrategyEnum]
			 */
			static dodoString zCompress(const dodoString &buffer, unsigned short level = 6, short type = TOOLS_ZLIB_HUFFMAN_COMRESSION);

			/**
			 * @return decompressed buffer
			 * @param buffer defines data to decompress
			 */
			static dodoString zDecompress(const dodoString &buffer);

#endif

			/**
			 * @return decoded URL
			 * @param string defines URL to decode
			 */
			static dodoString decodeURL(const dodoString &string);

			/**
			 * @return encoded URL
			 * @param string defines URL to encode
			 */
			static dodoString encodeURL(const dodoString &string);

			/**
			 * @return converted from HEX
			 * @param first defines first part of HEX
			 * @param second defines second part of HEX
			 */
			static char hexToChar(char first, char second);

			/**
			 * @param result defines converted to HEX
			 * @param first defines char to convert
			 */
			static void charToHex(char result[3], char first);

			/**
			 * @return encoded string with ASCII85 method
			 * @param string defines data to encode
			 */
			static dodoString encodeASCII85(const dodoString &string);

			/**
			 * @return decoded string with ASCII85 method
			 * @param string defines data to decode
			 */
			static dodoString decodeASCII85(const dodoString &string);

			/**
			 * @return encoded string with base64 method
			 * @param string defines data to encode
			 */
			static dodoString encodeBase64(const dodoString &string);

			/**
			 * @return decoded string with base64 method
			 * @param string defines data to decode
			 */
			static dodoString decodeBase64(const dodoString &string);

			/**
			 * @return parts of URL
			 * @param url defines URL
			 */
			static __url parseURL(const dodoString &url);

			/**
			 * @return MD5 hash of string
			 * @param string defines string for what generate hash
			 */
			static dodoString MD5(const dodoString &string);

			/**
			 * @return MD5 hash of string in 'hex' representation(e.g. 'fbc093901857fcd118f065f900982c24')
			 * @param string defines string for what generate hash
			 */
			static dodoString MD5Hex(const dodoString &string);

#ifdef BZIP2_EXT

			/**
			 * @return compressed buffer
			 * @param buffer defines data to compress
			 * @param level defines compression level[1..9]
			 * @param type defines compression strategy;controls how the compression phase behaves when presented with worst case, highly repetitive, input data[0..250]
			 */
			static dodoString bzCompress(const dodoString &buffer, unsigned short level = 6, unsigned short type = 30);

			/**
			 * @return decompressed buffer
			 * @param buffer defines data to decompress
			 */
			static dodoString bzDecompress(const dodoString &buffer);

#endif

			/**
			 * send mail using sendmail external program
			 * @param path defines path to sendmail
			 * @param to defines mail address[possible multiply separated with coma]
			 * @param subject defines a subject of the letter;for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`
			 * @param message defines a message to send
			 * @param headers defines extra headers
			 */
			static void mail(const dodoString &path, const dodoString &to, const dodoString &subject, const dodoString &message, const dodoString &headers = __dodostring__);

			/**
			 * send mail
			 * @param host defines host of smtp server(ip)
			 * @param type defines type of `socketProtoFamilyEnum`
			 * @param port defines port of smtp server
			 * @param to defines mail address where to send[possible multiply separated with coma]
			 * @param from defines mail address of sender
			 * @param subject defines a subject of the letter;for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`
			 * @param message defines a message to send
			 * @param login defines a login for auth
			 * @param pass defines a password for auth
			 * @param headers defines extra headers[each must ends with `\r\n`]
			 * @note if login is emty no auth is performed
			 */
			static void mail(const dodoString &host, short type, int port, const dodoString &to, const dodoString &from, const dodoString &subject, const dodoString &message, const dodoString &login = __dodostring__, const dodoString &pass = __dodostring__, const dodoString &headers = __dodostring__);

		private:

			/**
			 * decode block from base64
			 * @param in defines input block
			 * @param out defines result block
			 */
			static void _decodeBase64(unsigned char in[4], unsigned char out[3]);

			/**
			 * encode block to base64
			 * @param in defines input block
			 * @param out defines result block
			 * @param len defines size of 'in' block
			 */
			static void _encodeBase64(unsigned char in[3], unsigned char out[4], int len);

			/**
			 * encode tuple to ASCII85 and stores to result string
			 * @param result defines storage for encoded symbol[append]
			 * @param tuple defines part of data
			 * @param count defines size of data
			 */
			static void _encodeASCII85(dodoString &result, unsigned long tuple, int count);

			/**
			 * decode tuple from ASCII85 and stores to string
			 * @param result defines storage for decoded symbol[append]
			 * @param tuple defines part of encoded data
			 * @param count defines size of encoded data
			 */
			static void _decodeASCII85(dodoString &result, unsigned long tuple, int count);

			/**
			 * MD5 structure
			 */
			struct MD5_CTX
			{
				unsigned int state[4];      ///< state[A,B,C,D]
				unsigned int count[2];      ///< number of bits, modulo 2^64 (lsb first)
				unsigned char buffer[64];   ///< input buffer
			};

			/**
			 * init MD5 structure
			 * @param context defines MD5 structure
			 */
			static void MD5Init(MD5_CTX *context);

			/**
			 * MD5 block update operation
			 * @param context defines MD5 structure
			 * @param input defines input data
			 * @param inputLen defines size of input data 
			 * @note continues an MD5 message-digest operation, processing another message block, and updating the context.
			 */
			static void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);
			/**
			 * MD5 basic transformation
			 * @param state defines state of transformation[A,B,C,D]
			 * @param block defines transformation block
			 * @note transforms state based on block
			 */
			static void MD5Transform(unsigned int state[4], unsigned char block[64]);

			/**
			 * MD5 finalization
			 * @param digest defines digest
			 * @param context defines MD5 structure
			 * @note ends an MD5 message-digest operation, writing the the message digest and zeroizing the context
			 */
			static void MD5Final(unsigned char digest[16], MD5_CTX * context);

			/**
			 * dummy callback function for implode/explode
			 * @return result string
			 * @param data defines data to process
			 * @note nothing does with passed data
			 */
			inline static dodoString dummyTools(const dodoString &data);

			/**
			 * send data through socket
			 * @param socket defines socket descriptor
			 * @param mess defines data to send
			 * @note size of message must be not more than TOOLS_SHORT_DATA_SIZE bytes
			 */
			static void sendShortData(int socket, const dodoString &mess);

			/**
			 * send data through socket
			 * @param socket defines socket descriptor
			 * @param mess defines data to send
			 * @param data defines data to delete on exception
			 * @note size of message must be not more than TOOLS_SHORT_DATA_SIZE bytes
			 * on exception deletes data
			 */
			static void sendShortDataDel(int socket, const dodoString &mess, char *data);

			/**
			 * send data through socket
			 * @param socket defines socket descriptor
			 * @param mess defines data to send
			 * @note size of message must be not more than TOOLS_SHORT_DATA_SIZE bytes
			 */
			static void sendLongData(int socket, const dodoString &mess);

			/**
			 * receive data through socket
			 * @param socket defines socket descriptor
			 * @param data defines receive buffer
			 * @note size of data must be not more than TOOLS_SHORT_DATA_SIZE bytes
			 * on exception deletes data
			 */
			static void receiveShortDataDel(int socket, char *data);
	};

};
#endif