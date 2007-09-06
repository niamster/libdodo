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

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum toolsExR
	{
		TOOLS_BAD_ASCII85 = 1,
		TOOLS_BAD_BZCOMPRESSION,
		TOOLS_BAD_BZDECOMPRESSION_INIT,
		TOOLS_BAD_BZDECOMPRESSION_FINISH,
		TOOLS_BAD_BZDECOMPRESSION,
		TOOLS_BADMAILHELO,
		TOOLS_BADMAILAUTH,
		TOOLS_WRONG_PARAMETHER,

            #ifndef FAST

		TOOLS_DATA_TOO_LONG,

            #endif

		TOOLS_ARRAY_EMPTY
	};

	/**
	 * libdodo defined errors' explanation
	 */
        #define TOOLS_BAD_ASCII85_STR                       "Bad character in ASCII85."

        #ifdef BZIP2_EXT

            #define TOOLS_BAD_BZCOMPRESSION_STR             "Error occured during comression."
            #define TOOLS_BAD_BZDECOMPRESSION_INIT_STR      "Error occured during preparations for decompression."
            #define TOOLS_BAD_BZDECOMPRESSION_FINISH_STR    "Error occured during finishing decompression."
            #define TOOLS_BAD_BZDECOMPRESSION_STR           "Error occured during decompression."

        #endif

        #define TOOLS_BADMAILHELO_STR                       "Error occurd while sending EHLO."
        #define TOOLS_BADMAILAUTH_STR                       "Error during uthentification."
        #define TOOLS_WRONG_PARAMETHER_STR                  "Wrong paramather passed to function."
        #define TOOLS_DATA_TOO_LONG_STR                     "Data size is bigger than TOOLS_SHORT_DATA_SIZE."
        #define TOOLS_ARRAY_EMPTY_STR                       "Array is empty."

	/**
	 * ID of function where exception was thrown
	 */
	enum toolsFunctionsID
	{
            #ifdef ICONV_EXT

		TOOLS_CODESETCONVERSION,
		TOOLS_CODESETCONVERSIONSTATIC,
		TOOLS_RECODESETCONVERSION,
		TOOLS_CODESET,

            #endif

            #ifdef ZLIB_EXT

		TOOLS_ZCOMPRESS,
		TOOLS_ZDECOMPRESS,

            #endif

		TOOLS_DECODEASCII85,

            #ifdef BZIP2_EXT

		TOOLS_BZCOMPRESS,
		TOOLS_BZDECOMPRESS,

            #endif

		TOOLS_MAIL,
		TOOLS_SENDSHORTDATA,
		TOOLS_SENDSHORTDATADEL,
		TOOLS_RECEIVESHORTDATADEL,
		TOOLS_SENDLONGDATA,
		TOOLS_IMPLODE,
		TOOLS_RANDOM
	};

};

#endif

#endif
