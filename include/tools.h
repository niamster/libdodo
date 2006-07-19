/***************************************************************************
 *            tools.h
 *
 *  Wed Aug 24 00:42:51 2005
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
 
#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <directives.h>

#include <types.h>

namespace dodo
{
	/**
	 * @def adds bit flag to statement
	 */
	#define addF(flag,statement) (flag)|=(statement)
	/**
	 * @def removes bit flag to statement
	 */
	#define removeF(flag,statement) (flag)&=(~(statement))

	/**
	 * @typedef that describes function that will escapes parts in explode/implode
	 * @return converted string
	 */
	typedef std::string (*escape)(const std::string &);
		
	 #ifdef ZLIB_EXT
	 
	 	/**
	 	 * @enum zlibCompressionStrategyEnum
	 	 */
	 	enum zlibCompressionStrategyEnum
	 	{
	 		ZLIB_FILTRED_COMRESSION=1,
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
		std::string protocol;
		std::string host;
		std::string path;
		std::string request;
		std::string login;
		std::string password;
		std::string port;
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
			 * removes symbols from the end and from the begin of given string
			 * @return processed string
			 * @param data is string to process
			 * @param symbol is symbol to remove
			 */
			static std::string trim(const std::string &data, char symbol=' ');
						
			/**
			 * removes symbols[from given array] from the end and from the begin of given string
			 * @return processed string
			 * @param data is string to process
			 * @param symbols is an array of symbols to remove
			 * @param symCount is amount of symbols in @param symbols
			 */
			static std::string trim(const std::string &data, char symbols[], int symCount);
						
			/**
			 * removes symbols[from given array] from the end of given string
			 * @return processed string
			 * @param data is string to process
			 * @param symbols is an array of symbols to remove
			 * @param symCount is amount of symbols in @param symbols
			 */
			static std::string rTrim(const std::string &data, char symbols[], int symCount);
			
			/**
			 * removes symbols[from given array] from the begin of given string
			 * @return processed string
			 * @param data is string to process
			 * @param symbols is an array of symbols to remove
			 * @param symCount is amount of symbols in @param symbols
			 */
			static std::string lTrim(const std::string &data, char symbols[], int symCount);
						
			/**
			 * removes symbols from the end of given string
			 * @return processed string
			 * @param data is string to process
			 * @param symbol is symbol to remove
			 */
			static std::string rTrim(const std::string &data, char symbol=' ');
			
			/**
			 * removes symbols from the begin of given string
			 * @return processed string
			 * @param data is string to process
			 * @param symbol is symbol to remove
			 */
			static std::string lTrim(const std::string &data, char symbol=' ');
						
			/**
			 * @return converted long to string
			 * @param number is number to convert
			 */
			static std::string lToString(long number);
			
			/**
			 * @return converted double to string
			 * @param number is number to convert
			 */
			static std::string dToString(double number);
			
			/**
			 * replaces nodes in string
			 * @param needle indicates what to replace in string
			 * @param replacement indicates with what to replace
			 * @param data specifies wher to replace
			 */
			static void replace(pchar needle, pchar replacement, std::string &data);
		
			/**
			 * @return array of strings made from string using separtator
			 * @param fields is string to explode
			 * @param separator is value that separates parts
			 * @param limit describes what on how many parts to devide string;  -1 => unlimit
			 */
			static stringArr explode(const std::string &fields, const std::string &separator, int limit=-1);
					
			/**
			 * @return array of strings made from string using separtator
			 * @param escapeF is callback function that will process with every made node
			 * @param fields is string to explode
			 * @param separator is value that separates parts
			 * @param limit describes what on how many parts to devide string;  -1 => unlimit
			 */
			static stringArr explode(const std::string &fields, escape escapeF, const std::string &separator, int limit=-1);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param escapeF is callback function that will process with every node
			 * @param separator is value that separates parts
			 * @param frame is node that will cover node [frame='; node => 'node']
			 * @param limit describes what on how many parts to devide string;  -1 => unlimit
			 */
			static std::string implode(const stringArr &fields, escape escapeF, const std::string &separator, const std::string &frame, int limit=-1);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param escapeF is callback function that will process with every node
			 * @param separator is value that separates parts
			 * @param limit describes what on how many parts to devide string;  -1 => unlimit
			 */
			static std::string implode(const stringArr &fields, escape escapeF, const std::string &separator, int limit=-1);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param separator is value that separates parts
			 * @param limit describes what on how many parts to devide string;  -1 => unlimit
			 */
			static std::string implode(const stringArr &fields, const std::string &separator, int limit=-1);
			
			/**
			 * @return string of fields, separated with separator;
			 * @param fields is an array of nodes that will fiil the result string
			 * @param separator is value that separates parts
			 * @param frame is node that will cover node [frame='; node => 'node']
			 * @param limit describes what on how many parts to devide string;  -1 => unlimit
			 */
			static std::string implode(const stringArr &fields, const std::string &separator, const std::string &frame, int limit=-1);
			
			#ifdef CODECONV_EXT
				
				/**
				 * converts from one codeset to another
				 * @return converted string
				 * @param buffer contains string to convert
				 * @param toCode indicates codeset in what perform conversion
				 * @param fromCode indicates codeset in what buffer coded
				 * @note if compiled without exeptions - on error buffer will be returned
				 */
				static std::string codesetConversionStatic(const std::string &buffer, const std::string &toCode, const std::string &fromCode);
								
				/**
				 * converts from one codeset to another
				 * @return converted string
				 * @param buffer contains string to convert
				 * @param toCode indicates codeset in what perform conversion
				 * @param fromCode indicates codeset in what buffer coded
				 * @note if compiled without exeptions - on error buffer will be returned
				 */
				virtual std::string codesetConversion(const std::string &buffer, const std::string &toCode, const std::string &fromCode);

				/**
				 * converts from one codeset to another
				 * @return converted string
				 * @param buffer contains string to convert
				 * @note you may use it you called once codeSetConvert method. it'll be faster for future conversions
				 * usefull if you have only one type of conversion
				 * if compiled without exeptions - on error buffer will be returned
				 */
				virtual std::string reCodesetConversion(const std::string &buffer);
				
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
								codeSet(const std::string &toCode, const std::string &fromCode);
				
				
			 #endif
			 
			 #ifdef ZLIB_EXT
			 	
			 	/**
			 	 * @return compressed buffer
			 	 * @param buffer contains data to compress
			 	 * @param level is level to compress [1..9]
			 	 * @param type descibes compression strategy[see zlibCompressionStrategyEnum]
				 * @note if compiled without exeptions - on error buffer will be returned
			 	 */
			 	static std::string zCompress(const std::string &buffer, unsigned short level=6, short type=ZLIB_HUFFMAN_COMRESSION);
			 	
			 	/**
			 	 * @return decompressed buffer
			 	 * @param buffer contains data to decompress
				 * @note if compiled without exeptions - on error buffer will be returned
			 	 */
			 	static std::string zDecompress(const std::string &buffer);
			 				 
			 #endif
			
			/**
			 * @return base64 decoded string
			 * @param string to decode
			 */
			static std::string decodeURL(const std::string &string);

			/**
			 * @return base64 encoded string
			 * @param string to encode
			 */			
			static std::string encodeURL(const std::string &string);
						
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
			static std::string encodeASCII85(const std::string &string);
			
			/**
			 * @return decoded string from ASCII85 method
			 * @param string is data to decode
			 */
			static std::string decodeASCII85(const std::string &string);
			
			/**
			 * @return encoded string with base64 method
			 * @param string is data to encode
			 */
			static std::string encodeBase64(const std::string &string);
			
			/**
			 * @return decoded string from base64method
			 * @param string is data to decode
			 */
			static std::string decodeBase64(const std::string &string);
			
			/**
			 * @return struct that contains parts of URL
			 * @param url describes URL to parse
			 */
			static __url parseURL(const std::string &url);
			
			#ifdef BZIP_EXT
			
			 	
			 	/**
			 	 * @return compressed buffer
			 	 * @param buffer contains data to compress
			 	 * @param level is level to compress [1..9]
			 	 * @param type descibes compression strategy[0..250] => controls how the compression phase behaves when presented with worst case, highly repetitive, input data
				 * @note if compiled without exeptions - on error buffer will be returned
				 * Lower values of workFactor reduce the amount of effort the standard algorithm will expend before resorting to the fallback
			 	 */
			 	static std::string bzCompress(const std::string &buffer, unsigned short level=6, unsigned short type = 30);
			 	
			 	/**
			 	 * @return decompressed buffer
			 	 * @param buffer contains data to decompress
				 * @note if compiled without exeptions - on error buffer will be returned
			 	 */
			 	static std::string bzDecompress(const std::string &buffer);			
			
			#endif
			
			/**
			 * sends mail using sendmail
			 * @param path is path to sendmail
			 * @param to is mail address where to send[possible multiply separated with coma]
			 * @param subject is a subject of the letter
			 * @param message is a message to send
			 * @param login is a login for auth
			 * @param pass is a password for auth
			 * @param headers - extra headers
			 * @note if login is emty - no auth
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif				
							mail(const std::string &path, const std::string &to, const std::string &subject, const std::string &message, const std::string &headers = __string__);
			
			/**
			 * sends mail
			 * @param host is host of smtp server(ip)
			 * @param type is type of `socketProtoFamilyEnum`
			 * @param port is port of smtp server
			 * @param to is mail address where to send[possible multiply separated with coma]
			 * @param from is mail address of sender
			 * @param subject is a subject of the letter[for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`]
			 * @param message is a message to send
			 * @param headers - extra headers [each must ends with `\r\n`]
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif				
							mail(const std::string &host, short type, int port, const std::string &to, const std::string &from, const std::string &subject, const std::string &message, const std::string &login = __string__, const std::string &pass = __string__, const std::string &headers = __string__);
									
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
			static void _encodeASCII85(std::string &result, unsigned long tuple, int count);

			/**
			 * encodes tuple from ASCII85 and stores to string
			 * @param string is storage for encoded symbol[append]
			 * @param tuple is part of encoded data
			 * @param count is amount of encoded data
			 */
			static void _decodeASCII85(std::string &result, unsigned long tuple, int count);
			
			#ifdef CODECONV_EXT
			
				iconv_t conv;///< discriptor for code conversions
				bool convSet;///< indicates whether codeset was set
				
				size_t in;///< inbytesleft paramether for iconv
				size_t out;///< outbytesleft paramether for iconv
				char *inFake;///< to protect incomming string
				char *outFake;///< to protect outgoing string
				
			#endif		
			
			#ifdef BZIP_EXT
			
			#endif			
								
			std::string result;///< to store temporary result
							
			/**
			 * dummy callback function for implode/explode
			 * nothing does with passed data
			 */
			inline static std::string dummyTools(const std::string &data);			
	};

};
#endif /* _TOOLS_H */
