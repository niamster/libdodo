/***************************************************************************
 *            regexpTools.h
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

#include <directives.h>

#include <types.h>
#include <regexpToolsEx.h>
#include <dodoBase.h>

#ifdef PCRE_EXT
	
	#include <pcre.h>
	
#else
	
	#include <sys/types.h>
	#include <regex.h>
	
#endif

namespace dodo
{
	
	/**
	 * !class regexpTools that covers REGEXP routine using different regex libs
	 * @note PCRE is much faster
	 * both POSIX and PCRE don't support binary patterns
	 * if string is not matchin' fully the pattern - it don't want to execute it(return false)
	 * 
	 * POSIX doesn't support binary test sting;
	 */
	 class regexpTools
	 {
	 	public:
						 	
	 		/**
	 		 * constructor
	 		 */
	 		regexpTools();
	 		
	 		/**
	 		 * destructor
	 		 */
	 		virtual ~regexpTools();
	 		
	 		mutable bool extended;///< set whether to use extended or basic regex; extended by default

	 		mutable bool icase;///< ignore case; not active by default

	 		/**
	 		 * @return true if matched
	 		 * @param pattern is regex pattern
	 		 * @param sample is a test string
	 		 * @param pockets is array that will be filled with matched in '()' 
	 		 * @note set matched pieces in '()' to pockets
	 		 * pockets clears before fillin'
	 		 * first in pocket is not sample - but first match
	 		 */
	 		bool match(const std::string &pattern, const std::string &sample, stringArr &pockets = __stringarray__) const;

	 		/**
	 		 * matches with pattern prviously given with match method; if patterns are similar - faster!
	 		 * @return true if matched
	 		 * @param sample is a test string
	 		 * @param pockets is array that will be filled with matched in '()' 
	 		 * @note set matched pieces in '()' to pockets
	 		 * pockets clears before fillin'
	 		 * first in pocket is not sample - but first match
	 		 */
	 		bool reMatch(const std::string &sample, stringArr &pockets = __stringarray__) const;
	 		
	 		/**
	 		 * replaces in sample from pieces usin' pattern
	 		 * @return string with replacements
	 		 * @param pattern is regex pattern
	 		 * @param sample is a test string
	 		 * @param replacements is array that will fill parts with matched in '()' 
	 		 * @note if amount of pockets more than replacements  - replacemet will stop
	 		 * if pattern is not matched - the sample will be returned
	 		 */
	 		std::string replace(const std::string &pattern, const std::string &sample, const stringArr &replacements) const;
	 		
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
	 		std::string reReplace(const std::string &sample, const stringArr &replacements) const;
	 		
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
	 		bool boundMatch(const std::string &sample) const;
	 		
	 		/**
	 		 * compile pattern
	 		 */
	 		bool compile(const std::string &pattern) const;
	 	
	 	private:
	 	
			#ifdef PCRE_EXT
				mutable pcre *code;///< compiled pattern
			#else
				mutable regex_t code;///< compiled pattern
				mutable bool notCompiled;///< indicates, if not compiled
			#endif	 		
	 		
			/**
			 * @note share vars
			 */
			mutable std::list<__regexMatch> boundaries;///< list of buondaries matched in sample by pattern
			mutable std::list<__regexMatch>::const_iterator i;///< iterator for list of buondaries matched in sample by pattern
			mutable std::list<__regexMatch>::const_iterator j;///< iterator for list of buondaries matched in sample by pattern
	
			mutable __regexMatch bound;///< temporary to save bound
	 };
};

#endif /*REGEXP_*/
