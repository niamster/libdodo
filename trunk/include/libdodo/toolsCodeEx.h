/***************************************************************************
 *            toolsCodeEx.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _TOOLSCODEEX_H_
#define _TOOLSCODEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace tools {
		/**
		 * libdodo defined errors
		 */
		enum codeExR {
			CODEEX_BADASCII85,
			CODEEX_BADBZCOMPRESSION,
			CODEEX_BADBZDECOMPRESSIONINIT,
			CODEEX_BADBZDECOMPRESSIONFINISH,
			CODEEX_BADBZDECOMPRESSION,
		};

		/**
		 * explanations for libdodo defined errors
		 */
#define TOOLSCODEEX_BADASCII85_STR "Bad character in ASCII85."

#ifdef BZIP2_EXT

#define TOOLSCODEEX_BADBZCOMPRESSION_STR         "Error occured during comression."
#define TOOLSCODEEX_BADBZDECOMPRESSIONINIT_STR   "Error occured during preparations for decompression."
#define TOOLSCODEEX_BADBZDECOMPRESSIONFINISH_STR "Error occured during finishing decompression."
#define TOOLSCODEEX_BADBZDECOMPRESSION_STR       "Error occured during decompression."

#endif

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum codeFunctionsID {
#ifdef ICONV_EXT

			CODEEX_CODESETCONVERSION,

#endif

#ifdef ZLIB_EXT

			CODEEX_ZCOMPRESS,
			CODEEX_ZDECOMPRESS,

#endif

			CODEEX_DECODEASCII85,

#ifdef BZIP2_EXT

			CODEEX_BZCOMPRESS,
			CODEEX_BZDECOMPRESS,

#endif
		};
	};
};

#endif
