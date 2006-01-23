/***************************************************************************
 *            cgiPreprocessor.h
 *
 *  Sun Jan 22 19:05:57 2006
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef _CGIPREPROCESSOR_H_
#define _CGIPREPROCESSOR_H_

#include <directives.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <types.h>
#include <tools.h>
#include <cgiPreprocessorEx.h>

namespace dodo
{

	/**
	 * prepares template for future parsings
	 */
	class cgiPreprocessor
	{
		public:
							
			/**
			 * constructor
			 */
			cgiPreprocessor();
			
			/**
			 * destructor
			 */
			virtual ~cgiPreprocessor();
			
			/**
			 * @return template from file and parses
			 * @param path indicates path where template stays
			 */		
			virtual std::string preProcess(const std::string &path) const;
		
		protected:	
			
			/**
			 * parses preprocessor's part and stores into internal buffer
			 * @param buffer describes contents of template
			 * @param path indicates path where template stays
			 */		
			virtual std::string process(const std::string &buffer, std::string path) const;
			
			/**
			 * @return true if path is in `processed` list
			 */
			virtual bool recursive(const std::string &path) const;
			
			/**
			 * @return file contents
			 * @param path indicates what to read
			 */
			virtual std::string read(const std::string &path) const;
			
			mutable std::string temp;///< temp storage
			mutable std::string temp1;///< temp storage
			
			//mutable std::list<std::string> defNames;///< names of defines
			//mutable std::list<std::string> defVals;///< values of defines
			
			mutable std::list<std::string> processed;///< vector of files that will be skipped due to recurse
			
			mutable std::list<std::string>::iterator i;///< iterator for list
			mutable std::list<std::string>::iterator j;///< iterator for list
			
	};

};

#endif
