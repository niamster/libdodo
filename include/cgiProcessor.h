/***************************************************************************
 *            cgiProcessor.h
 *
 *  Tue Jan 24 19:05:57 2006
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
 
#ifndef _CGIPROCESSOR_H_
#define _CGIPROCESSOR_H_

#include <directives.h>

#include <types.h>
#include <tools.h>
#include <cgiProcessorEx.h>
#include <cgiPreprocessor.h>


namespace dodo
{
	/**
	 * prepares template for future parsings
	 */
	class cgiProcessor : public cgiPreprocessor
	{
		public:
							
			/**
			 * constructor
			 */
			cgiProcessor();
			
			/**
			 * destructor
			 */
			virtual ~cgiProcessor();
			
			/**
			 * @return parsed template from file
			 * @param path indicates path where template stays
			 */		
			virtual std::string process(const std::string &path);
					
			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable
			 */
			virtual void assign(const std::string &varName, const std::string &varVal);
		
		protected:	
			
			/**
			 * @return parsed template from preprocessored buffer
			 * @param path indicates buffer where template stays
			 * @param path indicates file where template got
			 */		
			virtual std::string _process(const std::string &buffer, const std::string &path);
			
			/**
			 * processes `if` statement
			 * @return position of cursor where to continue search
			 * @param buffer indicates what buffer contains found `if`
			 * @param start indicates where )> closes in `<( if ... )>` block
			 * @param statement indicates `if` statement
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long _if(const std::string &buffer, unsigned long start, const std::string &statement, std::string &tpl, const std::string &path);

			/**
			 * processes `include` statement
			 * @param buffer indicates what buffer contains found `if`
			 * @param statement indicates `include` statement
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual void _include(const std::string &statement, std::string &tpl, const std::string &path);
			
			/**
			 * @return position of the exact close block of the statement
			 * @param buffer indicates what buffer contains found `if`
			 * @param start indicates where )> closes if `<( if ... )>` block
			 * @param st is open statement[if, for ...]
			 * @param ts is close statement[fi, rof ...]
			 */
			virtual unsigned long blockEnd(const std::string &buffer, unsigned long start, const std::string &st, const std::string &ts);
						
			/**
			 * @return true if path is in `processed` list
			 * @param path desribes what to look up
			 */
			virtual bool recursive(const std::string &path);		
			
			/**
			 * @return var's value
			 * @param varName describes name of variable
			 */
			virtual std::string getVar(const std::string &varName);
			
			std::string temp;///< temp storage
			
			std::list<std::string> processed;///< vector of files that will be skipped due to recurse
			
			std::list<std::string>::iterator i;///< iterator for list
			std::list<std::string>::iterator j;///< iterator for list
			
			std::map<std::string, std::string> global;///< set of global variables[user-set]
			std::map<std::string, std::string> local;///< set of local variables[during parsing]
			
			std::map<std::string, std::string>::iterator k;///< iterator for map
			std::map<std::string, std::string>::iterator l;///< iterator for map
			
			std::string temp1;///< temporary storage

	};

};

#endif
