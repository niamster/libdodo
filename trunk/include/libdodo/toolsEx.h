/***************************************************************************
 *            toolsEx.h
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

#ifndef _TOOLSEX_H_
#define _TOOLSEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum toolsExR
	{
		TOOLSEX_BADASCII85 = 1,
		TOOLSEX_BADBZCOMPRESSION,
		TOOLSEX_BADBZDECOMPRESSION_INIT,
		TOOLSEX_BADBZDECOMPRESSION_FINISH,
		TOOLSEX_BADBZDECOMPRESSION,
		TOOLSEX_BADMAILHELO,
		TOOLSEX_BADMAILAUTH,
		TOOLSEX_WRONGPARAMETER,

			#ifndef FAST

		TOOLSEX_DATATOOLONG,

			#endif

		TOOLSEX_EMPTYARRAY,
		TOOLSEX_WRONGSTRENGTH
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define TOOLSEX_BADASCII85_STR                       "Bad character in ASCII85."

		#ifdef BZIP2_EXT

			#define TOOLSEX_BADBZCOMPRESSION_STR             "Error occured during comression."
			#define TOOLSEX_BADBZDECOMPRESSION_INIT_STR      "Error occured during preparations for decompression."
			#define TOOLSEX_BADBZDECOMPRESSION_FINISH_STR    "Error occured during finishing decompression."
			#define TOOLSEX_BADBZDECOMPRESSION_STR           "Error occured during decompression."

		#endif

		#define TOOLSEX_BADMAILHELO_STR                       "Error occurd while sending EHLO."
		#define TOOLSEX_BADMAILAUTH_STR                       "Error during uthentification."
		#define TOOLSEX_WRONGPARAMETER_STR                  "Wrong paramather passed to function."
		#define TOOLSEX_DATATOOLONG_STR                     "Data size is bigger than TOOLSEX_SHORT_DATA_SIZE."
		#define TOOLSEX_EMPTYARRAY_STR                       "Array is empty."
		#define TOOLSEX_WRONGSTRENGTH_STR                     "Wrong strength argument."

	/**
	 * ID of function where exception was thrown
	 */
	enum toolsFunctionsID
	{
			#ifdef ICONV_EXT

		TOOLSEX_CODESETCONVERSION,

			#endif

			#ifdef ZLIB_EXT

		TOOLSEX_ZCOMPRESS,
		TOOLSEX_ZDECOMPRESS,

			#endif

		TOOLSEX_DECODEASCII85,

			#ifdef BZIP2_EXT

		TOOLSEX_BZCOMPRESS,
		TOOLSEX_BZDECOMPRESS,

			#endif

		TOOLSEX_MAIL,
		TOOLSEX_SENDSHORTDATA,
		TOOLSEX_SENDSHORTDATADEL,
		TOOLSEX_RECEIVESHORTDATADEL,
		TOOLSEX_SENDLONGDATA,
		TOOLSEX_IMPLODE,
		TOOLSEX_RANDOM
	};

};

#endif
