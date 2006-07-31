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

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(array)
			 */
			virtual void assign(const std::string &varName, const stringArr &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(hash)
			 */
			virtual void assign(const std::string &varName, const assocArr &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(array of hashes)
			 */
			virtual void assign(const std::string &varName, const std::vector<assocArr> &varVal);
			
			/**
			 * show to stdout parsed template
			 * @param path indicates path where template stays
			 */		
			virtual void display(const std::string &path);
			
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
			 * processes `for` statement
			 * @return position of cursor where to continue search
			 * @param buffer indicates what buffer contains found `for`
			 * @param start indicates where )> closes in `<( for ... )>` block
			 * @param statement indicates `if` statement
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long _for(const std::string &buffer, unsigned long start, const std::string &statement, std::string &tpl, const std::string &path);
			
			/**
			 * processes `print` statement
			 * @param statement indicates `print` statement
			 * @param tpl indicates string where to add result
			 */
			virtual void _print(const std::string &statement, std::string &tpl);
			
			/**
			 * processes `break` statement
			 * @param statement indicates `break` statement
			 */
			virtual bool _break(const std::string &statement);
			
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

			/**
			 * @return extracted data(e.g. removes pairs of ",',`)
			 * @param statement describes statement that needs extraction from the pairs of ",',`
			 */
			virtual std::string trim(const std::string &statement);
			
			std::list<std::string> processed;///< vector of files that will be skipped due to recurse
			std::list<std::string>::iterator i;///< iterator for list
			std::list<std::string>::iterator j;///< iterator for list
		
			std::map<std::string, stringArr> globalArray;///< set of global variables(arrays)[user-set]
			std::map<std::string, stringArr>::iterator o;///< iterator for map of string arrays
			std::map<std::string, stringArr>::iterator p;///< iterator for map of string arrays
			
			std::map<std::string, assocArr> globalHash;///< set of global variables(hashes)[user-set]
			std::map<std::string, assocArr>::iterator g;///< iterator for map of maps
			std::map<std::string, assocArr>::iterator h;///< iterator for map of maps
			
			std::map<std::string, std::vector<assocArr> > globalArrayHash;///< set of global variables(array of hashes)[user-set]
			std::map<std::string, assocArr> localHash;///< set of local variables(hashes)
			std::map<std::string, std::vector<assocArr> >::iterator d;///< iterator for map of maps
			std::map<std::string, std::vector<assocArr> >::iterator f;///< iterator for map of maps
			
			assocArr dodo;///< set of auxillary variables[dodo defined][use as dodo.smth]
			
			assocArr global;///< set of global variables[user-set]
			assocArr local;///< set of local variables[during parsing]			
			assocArr::iterator k;///< iterator for map
			assocArr::iterator l;///< iterator for map
				
			std::string temp1;///< temporary storage									
			std::string temp;///< temp storage
			
			bool _continueFlag;///< indicates `continue`
			
			unsigned int _breakDeepness;///< deepness of the break
			unsigned int _loopDeepness;///< deepness of the loop
	};

};

#endif
