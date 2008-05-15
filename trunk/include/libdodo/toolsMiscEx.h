/***************************************************************************
 *            toolsMiscEx.h
 *
 *  Wed Oct 5 16:25:14 2005
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

#ifndef _TOOLSMISCEX_H_
#define _TOOLSMISKEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace tools
	{
		/**
		 * libdodo defined errors
		 */
		enum miscExR
		{
			MISCEX_BADASCII85 = 1,
			MISCEX_BADBZCOMPRESSION,
			MISCEX_BADBZDECOMPRESSIONINIT,
			MISCEX_BADBZDECOMPRESSIONFINISH,
			MISCEX_BADBZDECOMPRESSION,
			MISCEX_BADMAILHELO,
			MISCEX_BADMAILAUTH,
			MISCEX_WRONGPARAMETER,

	#ifndef FAST

			MISCEX_DATATOOLONG,

	#endif

			MISCEX_EMPTYARRAY,
			MISCEX_WRONGSTRENGTH
		};

		/**
		 * explanations for libdodo defined errors
		 */
	#define TOOLSMISCEX_BADASCII85_STR "Bad character in ASCII85."

	#ifdef BZIP2_EXT

	#define TOOLSMISCEX_BADBZCOMPRESSION_STR "Error occured during comression."
	#define TOOLSMISCEX_BADBZDECOMPRESSIONINIT_STR "Error occured during preparations for decompression."
	#define TOOLSMISCEX_BADBZDECOMPRESSIONFINISH_STR "Error occured during finishing decompression."
	#define TOOLSMISCEX_BADBZDECOMPRESSION_STR "Error occured during decompression."

	#endif

	#define TOOLSMISCEX_BADMAILHELO_STR "Error occurd while sending EHLO."
	#define TOOLSMISCEX_BADMAILAUTH_STR "Error during authentification."
	#define TOOLSMISCEX_WRONGPARAMETER_STR "Wrong paramather passed to function."
	#define TOOLSMISCEX_EMPTYARRAY_STR "Array is empty."
	#define TOOLSMISCEX_WRONGSTRENGTH_STR "Wrong strength argument."

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum miscFunctionsID
		{
	#ifdef ICONV_EXT

			MISCEX_CODESETCONVERSION,

	#endif

	#ifdef ZLIB_EXT

			MISCEX_ZCOMPRESS,
			MISCEX_ZDECOMPRESS,

	#endif

			MISCEX_DECODEASCII85,

	#ifdef BZIP2_EXT

			MISCEX_BZCOMPRESS,
			MISCEX_BZDECOMPRESS,

	#endif

			MISCEX_MAIL,
			MISCEX_IMPLODE,
			MISCEX_RANDOM
		};
	};
};

#endif
