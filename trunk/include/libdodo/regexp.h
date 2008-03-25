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
#include <libdodo/regexpEx.h>

namespace dodo
{

	/**
	 * @class regexp provides REGEXP support
	 * @note PCRE is much faster
	 * both POSIX and PCRE don't support binary patterns
	 * POSIX doesn't support binary match sting
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
			 * constructor
			 * @param pattern defines regex pattern
			 */
			regexp(const dodoString &pattern);

			/**
			 * destructor
			 */
			virtual ~regexp();

			/**
			 * @return true if matched pattern
			 * @param pattern defines regex pattern
			 * @param sample defines a match string
			 * @param pockets defines array that will be filled with matched substrings in '()'
			 * @note first in pocket is a first match
			 */
			bool match(const dodoString &pattern, const dodoString &sample, dodoStringArray &pockets = __dodostringarray__);

			/**
			 * match with pattern previously given for match/compile method
			 * @return true if matched pattern
			 * @param sample defines a match string
			 * @param pockets defines array that will be filled with matched substrings in '()'
			 * @note first in pocket is a first match
			 */
			bool match(const dodoString &sample, dodoStringArray &pockets = __dodostringarray__);

			/**
			 * replace substrings in match string
			 * @return string with replaced substrings
			 * @param pattern defines regex pattern
			 * @param sample defines a match string
			 * @param replacements defines array that will be filled with matched substrings in '()'
			 * @note if pattern is not matched - the sample will be returned
			 */
			dodoString replace(const dodoString &pattern, const dodoString &sample, const dodoStringArray &replacements);

			/**
			 * replace substrings in match string with pattern previously given for match/compile method
			 * @return string with replaced substrings
			 * @param sample defines a match string
			 * @param replacements defines array that will be filled with matched substrings in '()'
			 * @note if pattern is not matched - the sample will be returned
			 */
			dodoString replace(const dodoString &sample, const dodoStringArray &replacements);

			/**
			 * compile pattern
			 * @param pattern defines regex pattern
			 */
			void compile(const dodoString &pattern);

			bool extended;  ///< if true use extended regext support[true by default]

			bool icase;     ///< if true ignore case[false by default]

			bool greedy;    ///< if true REGEXPs are greedy[true by default]

			bool multiline; ///< if true match strings are treated as multiline[false by default]

		protected:

			/**
			 * @struct __regexMatch defines begin and end of matched substring
			 */
			struct __regexMatch
			{
				int begin;
				int end;
			};

			/**
			 * generate list of boundaries matched in match string by pattern
			 * @return true if match string has matched substrings
			 * @param sample defines a match string
			 */
			bool boundMatch(const dodoString &sample);

		private:

#ifdef PCRE_EXT
			pcre * code;            ///< compiled pattern
#else
			regex_t code;           ///< compiled pattern
			bool notCompiled;       ///< true if not compiled
#endif

			dodoArray<__regexMatch> boundaries; ///< list of boundaries matched in match string by pattern
	};

};

#endif
