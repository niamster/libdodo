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

#include <directives.h>

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

#include <flushSocketOptions.h>
#include <toolsEx.h>
#include <types.h>

namespace dodo
{
	/**
	 * @def adds bit flag to statement
	 */
    #define addFlag(statement, flag)       (statement) |= (flag)

	/**
	 * @def removes bit flag from statement
	 */
    #define removeFlag(statement, flag)    (statement) &= (~(flag))

	/**
	 * @def return true if bit flag is set
	 */
    #define isSetFlag(statement, flag)     ((statement) & (flag)) != 0

	/**
	 * @typedef that describes function that will escapes parts in explode/implode
	 * @return converted string
	 */
	typedef dodoString (*escape)(const dodoString &);

     #ifdef ZLIB_EXT

	/**
	 * @enum zlibCompressionStrategyEnum
	 */
	enum zlibCompressionStrategyEnum
	{
		ZLIB_FILTRED_COMRESSION = 1,
		ZLIB_HUFFMAN_COMRESSION,
		ZLIB_RLE_COMRESSION,
		ZLIB_FIXED_COMRESSION
	};

     #endif

	/**
	 * @struct __url describes info that could be retrieved from URL
	 */
	struct __url
	{
		dodoString protocol;
		dodoString host;
		dodoString path;
		dodoString request;
		dodoString login;
		dodoString password;
		dodoString port;
	};

	/**
	 * @class tools present different usefull functions
	 */
	class tools
	{
		private:

		/**
		 * copy constructor
		 * to prevent copying
		 */
		tools(tools &tls);

		public:

		/**
		 * constructor
		 */
		tools();

		/**
		 * destructor
		 */
		virtual ~tools();

		/**
		 * @return random data
		 * @param data is pointer where to store data
		 * @param size indicates amount of random data in bytes
		 * @note based on /dev/random
		 */
            #ifndef NO_EX
		static void
            #else
		static bool
            #endif
		random(void *data, unsigned long size);

		/**
		 * @return random unsigned long
		 */
		static unsigned long ulRandom();

		/**
		 * @return random long
		 */
		static long lRandom();

		/**
		 * @return random unsigned int
		 */
		static unsigned int uiRandom();

		/**
		 * @return random int
		 */
		static int iRandom();

		/**
		 * @return random unsigned short
		 */
		static unsigned short usRandom();

		/**
		 * @return random short
		 */
		static short sRandom();

		/**
		 * @return random unsigned char
		 */
		static unsigned char ucRandom();

		/**
		 * @return random char
		 */
		static char cRandom();

		/**
		 * @return random double
		 */
		static double dRandom();

		/**
		 * replaces nodes in string
		 * @param needle indicates what to replace in string
		 * @param replacement indicates with what to replace
		 * @param data specifies wher to replace
		 */
		static void replace(const dodoString &needle, const dodoString &replacement, dodoString &data);

		/**
		 * replaces nodes in string
		 * @param needle indicates what to replace in string
		 * @param replacement indicates with what to replace
		 * @param data specifies wher to replace
		 */
		static void replace(const dodoStringArr &needle, const dodoStringArr &replacement, dodoString &data);

		/**
		 * @return true if needle found in array
		 * @param arr is where to search
		 * @param needle is what to search for
		 * @param icase indicates whether to use or not caseless search
		 */
		static bool isInArray(const dodoStringArr &arr, const dodoString &needle, bool icase);

		/**
		 * @return array of strings made from string using separtator
		 * @param fields is string to explode
		 * @param separator is value that separates parts
		 * @param limit describes what on how many parts to devide string;  -1 => unlimit
		 */
		static dodoStringArr explode(const dodoString &fields, const dodoString &separator, int limit = -1);

		/**
		 * @return array of strings made from string using separtator
		 * @param escapeF is callback function that will process with every made node
		 * @param fields is string to explode
		 * @param separator is value that separates parts
		 * @param limit describes what on how many parts to devide string;  -1 => unlimit
		 */
		static dodoStringArr explode(const dodoString &fields, escape escapeF, const dodoString &separator, int limit = -1);

		/**
		 * @return string of fields, separated with separator;
		 * @param fields is an array of nodes that will fiil the result string
		 * @param escapeF is callback function that will process with every node
		 * @param separator is value that separates parts
		 * @param frame is node that will cover node [frame='; node => 'node']
		 * @param limit describes what on how many parts to devide string;  -1 => unlimit
		 */
		static dodoString implode(const dodoStringArr &fields, escape escapeF, const dodoString &separator, const dodoString &frame, int limit = -1);

		/**
		 * @return string of fields, separated with separator;
		 * @param fields is an array of nodes that will fiil the result string
		 * @param escapeF is callback function that will process with every node
		 * @param separator is value that separates parts
		 * @param limit describes what on how many parts to devide string;  -1 => unlimit
		 */
		static dodoString implode(const dodoStringArr &fields, escape escapeF, const dodoString &separator, int limit = -1);

		/**
		 * @return string of fields, separated with separator;
		 * @param fields is an array of nodes that will fiil the result string
		 * @param separator is value that separates parts
		 * @param limit describes what on how many parts to devide string;  -1 => unlimit
		 */
		static dodoString implode(const dodoStringArr &fields, const dodoString &separator, int limit = -1);

		/**
		 * @return string of fields, separated with separator;
		 * @param fields is an array of nodes that will fiil the result string
		 * @param separator is value that separates parts
		 * @param frame is node that will cover node [frame='; node => 'node']
		 * @param limit describes what on how many parts to devide string;  -1 => unlimit
		 */
		static dodoString implode(const dodoStringArr &fields, const dodoString &separator, const dodoString &frame, int limit = -1);

            #ifdef ICONV_EXT

		/**
		 * converts from one codeset to another
		 * @return converted string
		 * @param buffer contains string to convert
		 * @param toCode indicates codeset in what perform conversion
		 * @param fromCode indicates codeset in what buffer coded
		 * @note if compiled without exeptions - on error buffer will be returned
		 */
		static dodoString codesetConversionStatic(const dodoString &buffer, const dodoString &toCode, const dodoString &fromCode);

		/**
		 * converts from one codeset to another
		 * @return converted string
		 * @param buffer contains string to convert
		 * @param toCode indicates codeset in what perform conversion
		 * @param fromCode indicates codeset in what buffer coded
		 * @note if compiled without exeptions - on error buffer will be returned
		 */
		virtual dodoString codesetConversion(const dodoString &buffer, const dodoString &toCode, const dodoString &fromCode);

		/**
		 * converts from one codeset to another
		 * @return converted string
		 * @param buffer contains string to convert
		 * @note you may use it you called once codeSetConvert method. it'll be faster for future conversions
		 * usefull if you have only one type of conversion
		 * if compiled without exeptions - on error buffer will be returned
		 */
		virtual dodoString reCodesetConversion(const dodoString &buffer);

		/**
		 * set params for convertion [if you want to use reCodesetConversion wo calling codesetConversion before]
		 * @param toCode indicates codeset in what perform conversion
		 * @param fromCode indicates codeset in what buffer coded
		 */
                #ifndef NO_EX
		virtual void
                #else
		virtual bool
                #endif
		codeSet(const dodoString &toCode, const dodoString &fromCode);


             #endif

             #ifdef ZLIB_EXT

		/**
		 * @return compressed buffer
		 * @param buffer contains data to compress
		 * @param level is level to compress [1..9]
		 * @param type descibes compression strategy[see zlibCompressionStrategyEnum]
		 * @note if compiled without exeptions - on error buffer will be returned
		 */
		static dodoString zCompress(const dodoString &buffer, unsigned short level = 6, short type = ZLIB_HUFFMAN_COMRESSION);

		/**
		 * @return decompressed buffer
		 * @param buffer contains data to decompress
		 * @note if compiled without exeptions - on error buffer will be returned
		 */
		static dodoString zDecompress(const dodoString &buffer);

             #endif

		/**
		 * @return url decoded string
		 * @param string to decode
		 */
		static dodoString decodeURL(const dodoString &string);

		/**
		 * @return url encoded string
		 * @param string to encode
		 */
		static dodoString encodeURL(const dodoString &string);

		/**
		 * @return converted from HEX
		 * @param first is first part of HEX
		 * @param seconf is second part of HEX
		 */
		static char hexToChar(char first, char second);

		/**
		 * @return converted to HEX
		 * @param first is char to convert
		 */
		static void charToHex(char result[3], char first);

		/**
		 * @return encoded string with ASCII85 method
		 * @param string is data to encode
		 */
		static dodoString encodeASCII85(const dodoString &string);

		/**
		 * @return decoded string from ASCII85 method
		 * @param string is data to decode
		 */
		static dodoString decodeASCII85(const dodoString &string);

		/**
		 * @return encoded string with base64 method
		 * @param string is data to encode
		 */
		static dodoString encodeBase64(const dodoString &string);

		/**
		 * @return decoded string from base64method
		 * @param string is data to decode
		 */
		static dodoString decodeBase64(const dodoString &string);

		/**
		 * @return struct that contains parts of URL
		 * @param url describes URL to parse
		 */
		static __url parseURL(const dodoString &url);

		/**
		 * @return MD5 hash of string
		 * @param string indicates string for what generate hash
		 */
		static dodoString MD5(const dodoString &string);

		/**
		 * @return MD5 hash of string in 'hex' representation(e.g. 'fbc093901857fcd118f065f900982c24')
		 * @param string indicates string for what generate hash
		 */
		static dodoString MD5Hex(const dodoString &string);

            #ifdef BZIP2_EXT


		/**
		 * @return compressed buffer
		 * @param buffer contains data to compress
		 * @param level is level to compress [1..9]
		 * @param type descibes compression strategy[0..250] => controls how the compression phase behaves when presented with worst case, highly repetitive, input data
		 * @note if compiled without exeptions - on error buffer will be returned
		 * Lower values of workFactor reduce the amount of effort the standard algorithm will expend before resorting to the fallback
		 */
		static dodoString bzCompress(const dodoString &buffer, unsigned short level = 6, unsigned short type = 30);

		/**
		 * @return decompressed buffer
		 * @param buffer contains data to decompress
		 * @note if compiled without exeptions - on error buffer will be returned
		 */
		static dodoString bzDecompress(const dodoString &buffer);

            #endif

		/**
		 * sends mail using sendmail
		 * @param path is path to sendmail
		 * @param to is mail address where to send[possible multiply separated with coma]
		 * @param subject is a subject of the letter
		 * @param message is a message to send
		 * @param headers - extra headers
		 * @note if login is emty - no auth
		 */
            #ifndef NO_EX
		static void
            #else
		static bool
            #endif
		mail(const dodoString &path, const dodoString &to, const dodoString &subject, const dodoString &message, const dodoString &headers = __dodostring__);

		/**
		 * sends mail
		 * @param host is host of smtp server(ip)
		 * @param type is type of `socketProtoFamilyEnum`
		 * @param port is port of smtp server
		 * @param to is mail address where to send[possible multiply separated with coma]
		 * @param from is mail address of sender
		 * @param subject is a subject of the letter[for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`]
		 * @param message is a message to send
		 * @param login is a login for auth
		 * @param pass is a password for auth
		 * @param headers - extra headers [each must ends with `\r\n`]
		 */
            #ifndef NO_EX
		static void
            #else
		static bool
            #endif
		mail(const dodoString &host, short type, int port, const dodoString &to, const dodoString &from, const dodoString &subject, const dodoString &message, const dodoString &login = __dodostring__, const dodoString &pass = __dodostring__, const dodoString &headers = __dodostring__);

		private:

		/**
		 * decode block from base64
		 * @param in demonstrates input block
		 * @param in demonstrates result block
		 */
		static void _decodeBase64(unsigned char in[4], unsigned char out[3]);

		/**
		 * encode block to base64
		 * @param in demonstrates input block
		 * @param in demonstrates result block
		 * @param len is length of 'in' block
		 */
		static void _encodeBase64(unsigned char in[3], unsigned char out[4], int len);

		/**
		 * encodes tuple to ASCII85 and stores to string
		 * @param string is storage for encoded symbol[append]
		 * @param tuple is part of decoded data
		 * @param count is amount of encoded data
		 */
		static void _encodeASCII85(dodoString &result, unsigned long tuple, int count);

		/**
		 * encodes tuple from ASCII85 and stores to string
		 * @param string is storage for encoded symbol[append]
		 * @param tuple is part of encoded data
		 * @param count is amount of encoded data
		 */
		static void _decodeASCII85(dodoString &result, unsigned long tuple, int count);

            #ifdef ICONV_EXT

		iconv_t conv;           ///< discriptor for code conversions
		bool convSet;           ///< indicates whether codeset was set

            #endif

		/**
		 * MD5 structure
		 */
		struct MD5_CTX
		{
			unsigned int state[4];          ///< state (ABCD)
			unsigned int count[2];          ///< number of bits, modulo 2^64 (lsb first)
			unsigned char buffer[64];       ///< input buffer
		};

		/**
		 * inits MD5 structure
		 * @param contex describes MD5 structure
		 */
		static void MD5Init(MD5_CTX *context);

		/**
		 *  MD5 block update operation. Continues an MD5 message-digest
		 * operation, processing another message block, and updating the context.
		 */
		static void MD5Update(MD5_CTX *context, unsigned char *input, unsigned int inputLen);
		/**
		 * MD5 basic transformation. Transforms state based on block.
		 *
		 */
		static void MD5Transform(unsigned int state[4], unsigned char block[64]);

		/**
		 * MD5 finalization. Ends an MD5 message-digest operation, writing the
		 * the message digest and zeroizing the context.
		 * @param digest is digest =)
		 * @param contex describes MD5 structure
		 */
		static void MD5Final(unsigned char digest[16], MD5_CTX * context);

		/**
		 * dummy callback function for implode/explode
		 * nothing does with passed data
		 */
		inline static dodoString dummyTools(const dodoString &data);

		/**
		 * sends data through socket
		 * @param socket is socket descriptor
		 * @param mess is data to send
		 * @note mess's length not more than TOOLS_SHORT_DATA_SIZE bytes
		 */
            #ifndef NO_EX
		static void
            #else
		static bool
            #endif
		sendShortData(int socket, const dodoString &mess);

		/**
		 * sends data through socket
		 * @param socket is socket descriptor
		 * @param mess is data to send
		 * @param data what to delete on exception
		 * @note mess's length not more than TOOLS_SHORT_DATA_SIZE bytes
		 * on exception deletes data
		 */
            #ifndef NO_EX
		static void
            #else
		static bool
            #endif
		sendShortDataDel(int socket, const dodoString &mess, char *data);

		/**
		 * sends data through socket
		 * @param socket is socket descriptor
		 * @param mess is data to send
		 * @note mess's length can be more than TOOLS_SHORT_DATA_SIZE bytes
		 */
            #ifndef NO_EX
		static void
            #else
		static bool
            #endif
		sendLongData(int socket, const dodoString &mess);

		/**
		 * receives data through socket
		 * @param socket is socket descriptor
		 * @param data is where to receive
		 * @note data's length not more than TOOLS_SHORT_DATA_SIZE bytes
		 * on exception deletes data
		 */
            #ifndef NO_EX
		static void
            #else
		static bool
            #endif
		receiveShortDataDel(int socket, char *data);
	};

};
#endif
