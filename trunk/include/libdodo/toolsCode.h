/***************************************************************************
 *            toolsCode.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _TOOLSCODE_H_
#define _TOOLSCODE_H_ 1

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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdlib.h>

#include <libdodo/types.h>
#include <libdodo/ioNetwork.h>
#include <libdodo/toolsNetwork.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsCodeEx.h>

namespace dodo
{
	namespace cgi
	{
		class dialogue;
	};

	namespace io
	{
		namespace network
		{
			class http;
		};
	};

	namespace tools
	{
		/**
		 * @struct __url
		 * @brief defines Url parts
		 */
		struct __url
		{
			dodoString protocol;                            ///< scheme
			dodoString host;                                ///< host name
			dodoString path;                                ///< path to CGI
			dodoString request;                             ///< CGI request
			dodoString login;                               ///< login
			dodoString password;                            ///< password
			dodoString port;                                ///< port
		};

#ifdef ZLIB_EXT

		/**
		 * @enum codeZlibCompressionStrategyEnum
		 * @brief defines type of compression for zlib
		 */
		enum codeZlibCompressionStrategyEnum
		{
			CODE_ZLIB_FILTRED_COMRESSION = 1,
			CODE_ZLIB_HUFFMAN_COMRESSION,
			CODE_ZLIB_RLE_COMRESSION,
			CODE_ZLIB_FIXED_COMRESSION
		};

#endif

		/**
		 * @class code
		 * @brief provides encode/decode functionality
		 */
		class code
		{
			friend class io::network::http;
			friend class cgi::dialogue;
			friend class network;

			public:

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
				 * @param type defines compression strategy[see codeZlibCompressionStrategyEnum]
				 */
				static dodoString zCompress(const dodoString &buffer, unsigned short level = 6, short type = CODE_ZLIB_HUFFMAN_COMRESSION);

				/**
				 * @return decompressed buffer
				 * @param buffer defines data to decompress
				 */
				static dodoString zDecompress(const dodoString &buffer);

#endif

				/**
				 * @return decoded Url
				 * @param string defines Url to decode
				 */
				static dodoString decodeUrl(const dodoString &string);

				/**
				 * @return encoded Url
				 * @param string defines Url to encode
				 */
				static dodoString encodeUrl(const dodoString &string);

				/**
				 * @return converted from HEX
				 * @param first defines first part of HEX
				 * @param second defines second part of HEX
				 */
				static char hexToChar(char first, char second);

				/**
				 * @return numeric converted from HEX
				 * @param first defines numeric in HEX
				 */
				static unsigned long hexToLong(const dodoString &string);

				/**
				 * @return numeric converted to HEX
				 * @param first defines numeric
				 */
				static dodoString longToHex(unsigned long numeric);

				/**
				 * @param result defines converted to HEX
				 * @param character defines char to convert
				 */
				static void charToHex(char result[3], char character);

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
				 * @return parts of Url
				 * @param url defines Url
				 */
				static __url parseUrl(const dodoString &url);

				/**
				 * @return Url
				 * @param url defines Url parts
				 */
				static dodoString makeUrl(const __url &url);

				/**
				 * @return MD5 hash of string
				 * @param string defines string for what generate hash
				 */
				static dodoString MD5(const dodoString &string);

				/**
				 * @return MD5 hash of string in 'hex' representation(e.g. 'fbc093901857fcd118f065f900982c24')
				 * @param string defines string to convert
				 */
				static dodoString MD5Hex(const dodoString &string);

				/**
				 * @return SHA-1 hash of string
				 * @param string defines string for what generate hash
				 */
				static dodoString SHA1(const dodoString &string);

				/**
				 * @return SHA-1 hash of string in 'hex' representation(e.g. 'da39a3ee5e6b4b0d3255bfef95601890afd80709')
				 * @param string defines string to convert
				 */
				static dodoString SHA1Hex(const dodoString &string);

				/**
				 * @return SHA-256 hash of string
				 * @param string defines string for what generate hash
				 */
				static dodoString SHA256(const dodoString &string);

				/**
				 * @return SHA-256 hash of string in 'hex' representation(e.g. 'e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855')
				 * @param string defines string to convert
				 */
				static dodoString SHA256Hex(const dodoString &string);

				/**
				 * @return SHA-512 hash of string
				 * @param string defines string for what generate hash
				 */
				static dodoString SHA512(const dodoString &string);

				/**
				 * @return SHA-512 hash of string in 'hex' representation(e.g. 'cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e')
				 * @param string defines string to convert
				 */
				static dodoString SHA512Hex(const dodoString &string);

				/**
				 * @return binary string represented in 'hex'
				 * @param string defines string to convert
				 */
				static dodoString binToHex(const dodoString &string);

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

				static const char base64EncodeChars[65];///< base64 encoding characters
				static const char base64DecodeChars[81];///< base64 decoding characters

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

				static const unsigned long powASCII85[5];///< power values for ASCII85 encoding/decoding

				/**
				 * @struct __MD5Context
				 * @brief defines states for MD5 computations
				 */
				struct __MD5Context
				{
					unsigned int state[4];                                                  ///< state[A,B,C,D]
					unsigned int count[2];                                                  ///< number of bits, modulo 2^64 (lsb first)
					unsigned char buffer[64];                                               ///< input buffer
				};

				/**
				 * init MD5 context
				 * @param context defines MD5 structure
				 */
				static void MD5Init(__MD5Context *context);

				/**
				 * MD5 block update operation
				 * @param context defines MD5 context
				 * @param input defines input data
				 * @param inputLen defines size of input data
				 * @note continues an MD5 message-digest operation, processing another message block, and updating the context.
				 */
				static void MD5Update(__MD5Context *context, const unsigned char *input, unsigned int inputLen);
				/**
				 * MD5 basic transformation
				 * @param state defines state of transformation[A,B,C,D]
				 * @param block defines transformation block
				 * @note transforms state based on block
				 */
				static void MD5Transform(unsigned int state[4], const unsigned char block[64]);

				/**
				 * MD5 finalization
				 * @param digest defines digest
				 * @param context defines MD5 context
				 * @note ends an MD5 message-digest operation, writing the the message digest and zeroizing the context
				 */
				static void MD5Final(unsigned char digest[16], __MD5Context * context);

				static const unsigned char MD5Padding[64];///< MD5 padding for finalization stage

				static const char hexEncodeChars[17];///< characters for hex conversion

				/**
				 * @struct __SHAContext
				 * @brief defines states for SHA-1, SHA-256, SHA-512 computations
				 */
				struct __SHAContext
				{
					unsigned long intermediateHash[16]; ///< message digest[5 for SHA-1, 8 for SHA-256, 16 for SHA-512]

					unsigned long lengthLow; ///< message length in bits for SHA-1 and SHA-256
					unsigned long lengthHigh; ///< message length in bits for SHA-1 and SHA-256
					unsigned long length[4]; ///< message length in bits for SHA-512

					unsigned int messageBlockIndex; ///< index of messageBlock

					unsigned char messageBlock[128]; ///< 512-bit message blocks for SHA-1 and SHA-256, 1024-bit message blocks for SHA-512

					bool corrupted; ///< true if the digest corrupted
				};

				/**
				 * init SHA-1
				 * @param context defines SHA-1 context
				 */
				static void SHA1Init(__SHAContext *context);

				/**
				 * accepts an array of octets as the next portion of the message
				 * @param context defines SHA-1 context
				 * @param bytes defines input data
				 * @param bytecount defines size of input data
				 */
				static void SHA1Input(__SHAContext *context, const unsigned char *bytes, unsigned int bytecount);

				/**
				 * SHA-1 finalization
				 * @param context defines SHA-1 context
				 * @param digest defines digest
				 */
				static void SHA1Result(__SHAContext *context, unsigned char digest[20]);

				/**
				 * @param context defines SHA-1 context
				 * @param padByte defines the last byte to add to the digest before the 0-padding and length. This will contain the last bits of the message followed by another single bit. If the message was an exact multiple of 8-bits long, padByte will be 0x80
				 * @note according to the standard, the message must be padded to an even 512 bits.
				 * The first padding bit must be a '1'. The last 64 bits represent the length of the original message.
				 * All bits in between should be 0. This helper function will pad the message according to those rules by filling the messageBlock
				 * array accordingly. When it returns, it can be assumed that the message digest has been computed.
				 */
				static void SHA1PadMessage(__SHAContext *context, unsigned char padByte);

				/**
				 * processes the next 512 bits of the message stored in the messageBlock array in context
				 * @param context defines SHA-1 context
				 */
				static void SHA1ProcessMessageBlock(__SHAContext *context);

				/**
				 * init SHA-256
				 * @param context defines SHA-256 context
				 */
				static void SHA256Init(__SHAContext *context);

				/**
				 * accepts an array of octets as the next portion of the message
				 * @param context defines SHA-256 context
				 * @param bytes defines input data
				 * @param bytecount defines size of input data
				 */
				static void SHA256Input(__SHAContext *context, const unsigned char *bytes, unsigned int bytecount);

				/**
				 * SHA-256 finalization
				 * @param context defines SHA-256 context
				 * @param digest defines digest
				 */
				static void SHA256Result(__SHAContext *context, unsigned char digest[32]);

				/**
				 * @param context defines SHA-256 context
				 * @param padByte defines the last byte to add to the digest before the 0-padding and length. This will contain the last bits of the message followed by another single bit. If the message was an exact multiple of 8-bits long, padByte will be 0x80
				 * @note according to the standard, the message must be padded to an even 512 bits.
				 * The first padding bit must be a '1'. The last 64 bits represent the length of the original message.
				 * All bits in between should be 0. This helper function will pad the message according to those rules by filling the messageBlock
				 * array accordingly. When it returns, it can be assumed that the message digest has been computed.
				 */
				static void SHA256PadMessage(__SHAContext *context, unsigned char padByte);

				/**
				 * processes the next 512 bits of the message stored in the messageBlock array in context
				 * @param context defines SHA-256 context
				 */
				static void SHA256ProcessMessageBlock(__SHAContext *context);

				/**
				 * init SHA-512
				 * @param context defines SHA-512 context
				 */
				static void SHA512Init(__SHAContext *context);

				/**
				 * accepts an array of octets as the next portion of the message
				 * @param context defines SHA-512 context
				 * @param bytes defines input data
				 * @param bytecount defines size of input data
				 */
				static void SHA512Input(__SHAContext *context, const unsigned char *bytes, unsigned int bytecount);

				/**
				 * SHA-512 finalization
				 * @param context defines SHA-512 context
				 * @param digest defines digest
				 */
				static void SHA512Result(__SHAContext *context, unsigned char digest[64]);

				/**
				 * @param context defines SHA-512 context
				 * @param padByte defines the last byte to add to the digest before the 0-padding and length. This will contain the last bits of the message followed by another single bit. If the message was an exact multiple of 8-bits long, padByte will be 0x80
				 * @note according to the standard, the message must be padded to an even 1024 bits.
				 * The first padding bit must be a '1'. The last 128 bits represent the length of the original message.
				 * All bits in between should be 0. This helper function will pad the message according to those rules by filling the messageBlock
				 * array accordingly. When it returns, it can be assumed that the message digest has been computed.
				 */
				static void SHA512PadMessage(__SHAContext *context, unsigned char padByte);

				/**
				 * processes the next 512 bits of the message stored in the messageBlock array in context
				 * @param context defines SHA-512 context
				 */
				static void SHA512ProcessMessageBlock(__SHAContext *context);
		};
	};
};
#endif
