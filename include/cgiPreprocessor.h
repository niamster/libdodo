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

#include <types.h>

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
			 * @return preprocessed template from file
			 * @param path indicates path where template stays
			 */		
			virtual std::string preProcess(const std::string &path);
		
		protected:	
						
			/**
			 * @return line number in file
			 * @param newLinePos describes array of newlines' positions
			 * @param pos describes pos in file
			 */
			virtual unsigned long getLineNumber(const std::vector<unsigned long> &newLinePos, unsigned long pos);
			
			/**
			 * @return file contents
			 * @param path indicates what to read
			 * @param newLinePos describes array of newlines' positions
			 */
			virtual std::string read(const std::string &path, std::vector<unsigned long> &newLinePos);
			
			std::string temp;///< temp storage

			std::vector<unsigned long>::const_iterator o;///< iterator for vector
			std::vector<unsigned long>::const_iterator p;///< iterator for vector

			char message[128];

	};

};

#endif
