/***************************************************************************
 *            regexp.h
 *
 *  Thu Sep 20 01:39:24 2005
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


#ifndef _REGEXP_H_
#define _REGEXP_H_

#include <libdodo/directives.h>

#ifdef PCRE_EXT

#include <pcre.h>

#else

#include <sys/types.h>
#include <regex.h>

#endif

#include <libdodo/types.h>

namespace dodo
{

	/**
	 * @class regexp that covers REGEXP routine using different regex libs
	 * @note PCRE is much faster
	 * both POSIX and PCRE don't support binary patterns
	 * if string is not matchin' fully the pattern - it don't want to execute it(return false)
	 *
	 * POSIX doesn't support binary test sting;
	 */
	class regexp
	{

		private:

			/**
			 * copy constructor
			 * to prevent copying
			 */
			regexp(regexp &rt);

		public:

			/**
			 * constructor
			 */
			regexp();

			/**
			 * destructor
			 */
			virtual ~regexp();

			bool extended;  ///< set whether to use extended or basic regex; extended by default

			bool icase;     ///< ignore case; not active by default

			bool greedy;    ///< Invert greediness of quantifiers; greedy by default

			bool multiline; ///< multiline samples; false by default

			/**
			 * @return true if matched
			 * @param pattern is regex pattern
			 * @param sample is a test string
			 * @param pockets is array that will be filled with matched in '()'
			 * @note set matched pieces in '()' to pockets
			 * pockets clears before fillin'
			 * first in pocket is not sample - but first match
			 */
			bool match(const dodoString &pattern, const dodoString &sample, dodoStringArray &pockets = __dodostringarray__);

			/**
			 * matches with pattern prviously given with match method; if patterns are similar - faster!
			 * @return true if matched
			 * @param sample is a test string
			 * @param pockets is array that will be filled with matched in '()'
			 * @note set matched pieces in '()' to pockets
			 * pockets clears before fillin'
			 * first in pocket is not sample - but first match
			 */
			bool reMatch(const dodoString &sample, dodoStringArray &pockets = __dodostringarray__);

			/**
			 * replaces in sample from pieces usin' pattern
			 * @return string with replacements
			 * @param pattern is regex pattern
			 * @param sample is a test string
			 * @param replacements is array that will fill parts with matched in '()'
			 * @note if amount of pockets more than replacements  - replacemet will stop
			 * if pattern is not matched - the sample will be returned
			 */
			dodoString replace(const dodoString &pattern, const dodoString &sample, const dodoStringArray &replacements);

			/**
			 * matches with pattern prviously given with replace method; if patterns are similar - faster!
			 * replaces in sample from pieces usin' pattern
			 * @return string with replacements
			 * @param pattern is regex pattern
			 * @param sample is a test string
			 * @param replacements is array that will fill parts with matched in '()'
			 * @note if amount of pockets more than replacements  - replacemet will stop
			 * if pattern is not matched - the sample will be returned
			 */
			dodoString reReplace(const dodoString &sample, const dodoStringArray &replacements);

			/**
			 * compile pattern [if you want to use reReplace/reMatch wo calling replace/match before]
			 * @param pattern is regex pattern
			 */
			bool compile(const dodoString &pattern);

		protected:

			/**
			 * @struct __regexMatch indicates begin and end of matched data
			 */
			struct __regexMatch
			{
				int begin;
				int end;
			};

			/**
			 * generate list of boundaries matched in sample by pattern
			 */
			bool boundMatch(const dodoString &sample);

		private:

#ifdef PCRE_EXT
			pcre * code;            ///< compiled pattern
#else
			regex_t code;           ///< compiled pattern
			bool notCompiled;       ///< indicates, if not compiled
#endif

			dodoList<__regexMatch> boundaries; ///< list of buondaries matched in sample by pattern
	};

};

#endif
