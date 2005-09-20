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

#ifdef PCRE_EXT
	
	#include <pcre.h>
	
#else
	
	#include <sys/types.h>
	#include <regex.h>
	
#endif

namespace dodo
{
	/**
	 * class that covers REGEXP routine;
	 * usin' define flags u can compile with POSIX regex or with PCRE;
	 * PCRE is much faster, but it cannot be on your computer, but POSIX regex have to be(i hope!)
	 */
	 
	 class regexp
	 {
	 	
	 	public:
	 		/**
	 		 * matches and set matched pieces in '()' to pockets or return false;
	 		 */
	 		bool compile(const std::string &pattern, const std::string &sample, stringArr &pockets);
	 		/**
	 		 * replaces in sample from pieces usin' pattern
	 		 */
	 		std::string replace(const std::string &pattern, const std::string &sample, const stringArr &matches);
	 };
};

#endif /*REGEXP_*/
