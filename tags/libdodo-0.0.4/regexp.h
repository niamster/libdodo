/***************************************************************************
 *            regexp.h
 *
 *  Thu Sep 20 01:39:24 2005
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


#ifndef _REGEXP_H_
#define _REGEXP_H_

#include "directives.h"
#include "types.h"
#include "regexpEx.h"

#ifdef PCRE_EXT
	
	#include <pcre.h>
	
#else
	
	#include <sys/types.h>
	#include <regex.h>
	
#endif

namespace dodo
{
	
	/**
	 * 
	 */
	struct __regexMatch
	{
		int begin;
		int end;
	};
	
	/**
	 * class that covers REGEXP routine;
	 * usin' define flags u can compile with POSIX regex or with PCRE;
	 * PCRE is much faster, but it cannot be on your computer, but POSIX regex have to be(i hope!)
	 * 
	 *  if string is not matchin' fully the pattern - it don't want to execute it(return false)
	 * 
	 * NOTE!!!!!!! POSIX bugs: don't support binary test sting;
	 */
	 
	 
	 class regexp
	 {
	 	
	 	public:
	 	
	 		/**
	 		 * constructor/destructor
	 		 */
	 		regexp();
	 		virtual ~regexp();
	 		/**
	 		 * set whether to use Extended or basic regex; Extended by default
	 		 */ 
	 		mutable bool extended;
	 		/**
	 		 * ignore case
	 		 * not active by default
	 		 */
	 		mutable bool icase;
	 		/**
	 		 * matches and set matched pieces in '()' to pockets or return false;
	 		 * pockets clears before fillin'
	 		 * first in pocket is not sample - but first match
	 		 */
	 		bool match(const std::string &pattern, const std::string &sample, stringArr &pockets = __stringarry__) const;
	 		/**
	 		 * matches and set matched pieces in '()' to pockets usin' pattern from `match` or return false; faster than usage `exec` for some times with the same pattern
	 		 * pockets clears before fillin'
	 		 * first in pocket is not sample - but first match
	 		 */
	 		bool reMatch(const std::string &sample, stringArr &pockets = __stringarry__) const;
	 		/**
	 		 * replaces in sample from pieces usin' pattern
	 		 * if amount of pockets more than replacements  - replacemet will stop
	 		 * if pattern is not matched - the sample will be returned
	 		 */
	 		std::string replace(const std::string &pattern, const std::string &sample, const stringArr &replacements) const;
	 		/**
	 		 * replaces in sample from pieces usin' pattern;faster than usage `replace` for some times with the same pattern
	 		 * if amount of pockets more than replacements  - replacemet will stop
	 		 * if pattern is not matched - the sample will be returned
	 		 */
	 		std::string reReplace(const std::string &sample, const stringArr &replacements) const;
	 		
	 	protected:
	 	
	 		bool boundMatch(const std::string &sample) const;
	 		bool compile(const std::string &pattern) const;
	 	
			#ifdef PCRE_EXT
				mutable pcre *code;
			#else
				mutable regex_t code;
				mutable bool notCompiled;
			#endif	 		
	 		
			mutable std::list<__regexMatch> boundaries;
	 };
};

#endif /*REGEXP_*/
