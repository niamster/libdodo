/***************************************************************************
 *            cgiProcessor.h
 *
 *  Tue Jan 24 19:05:57 2006
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
 
#ifndef _CGIPROCESSOR_H_
#define _CGIPROCESSOR_H_

#include <directives.h>

#include <cgiProcessorEx.h>
#include <types.h>
#include <tools.h>
#include <cgiPreprocessor.h>
#include <cgiFast.h>

namespace dodo
{
	/**
	 * @class cgiProcessor prepares template for future parsings
	 */
	class cgiProcessor : public cgiPreprocessor
	{
		public:
							
			/**
			 * constructor
			 */
			cgiProcessor();
						
			#ifdef FCGI_EXT		
					
				/**
				 * constructor
				 * @param cf describes output interface 
				 */
				cgiProcessor(cgiFastSTD *cf);
			
			#endif
			
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
			 * @return parsed template from previously called preprocess
			 * @param path indicates path where template stays
			 * @note useful in fastCGI
			 */		
			virtual std::string reProcess();
					
			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable
			 */
			virtual void assign(std::string varName, const std::string &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(array)
			 */
			virtual void assign(std::string varName, const dodoStringArr &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(hash)
			 */
			virtual void assign(std::string varName, const dodoAssocArr &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(array of hashes)
			 */
			virtual void assign(std::string varName, const dodoArray<dodoAssocArr> &varVal);
			
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
			 * @param statement indicates `for` statement
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long _for(const std::string &buffer, unsigned long start, const std::string &statement, std::string &tpl, const std::string &path);

			/**
			 * processes `for` statement
			 * @return position of cursor where to continue search
			 * @param buffer indicates what buffer contains found `for`
			 * @param start indicates where )> closes in `<( namespace ... )>` block
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long _ns(const std::string &buffer, unsigned long start, std::string &tpl, const std::string &path);
						
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
			 * processes `assign` statement
			 * @param statement indicates `assign` statement
			 */
			virtual void _assign(const std::string &statement);
			
			/**
			 * processes `include` statement
			 * @param buffer indicates what buffer contains found `if`
			 * @param statement indicates `include` statement
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual void _include(const std::string &statement, std::string &tpl, const std::string &path);
			
			/**
			 * cleans namespace variable and back to life vars of prevous namespace that were overwritten
			 */
			virtual void cleanNamespace();
			
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
		
			std::map<std::string, dodoStringArr> globalArray;///< set of global variables(arrays)[user-set]
			std::map<std::string, dodoStringArr>::iterator o;///< iterator for map of string arrays
			std::map<std::string, dodoStringArr>::iterator p;///< iterator for map of string arrays
			
			std::map<std::string, dodoAssocArr> globalHash;///< set of global variables(hashes)[user-set]
			std::map<std::string, dodoAssocArr>::iterator g;///< iterator for map of maps
			std::map<std::string, dodoAssocArr>::iterator h;///< iterator for map of maps
			
			std::map<std::string, dodoArray<dodoAssocArr> > globalArrayHash;///< set of global variables(array of hashes)[user-set]
			std::map<std::string, dodoAssocArr> localHash;///< set of local variables(hashes)
			std::map<std::string, dodoArray<dodoAssocArr> >::iterator d;///< iterator for map of maps
			std::map<std::string, dodoArray<dodoAssocArr> >::iterator f;///< iterator for map of maps
			
			dodoAssocArr dodo;///< set of auxillary variables[dodo defined][use as dodo.smth]
			
			dodoAssocArr global;///< set of global variables[user-set]
			dodoAssocArr local;///< set of local variables[during parsing]			
			dodoAssocArr::iterator k;///< iterator for map
			dodoAssocArr::iterator l;///< iterator for map
				
			std::string temp1;///< temporary storage									
			std::string temp;///< temp storage
			
			bool _continueFlag;///< indicates `continue`
			
			unsigned int _breakDeepness;///< deepness of the break
			unsigned int _loopDeepness;///< deepness of the loop
			
			unsigned long iterator;///< count of iteration of a loop
			
			unsigned int namespaceDeepness;///< deepness of the namespace
			std::map<unsigned int, dodoAssocArr> localNamespace;///< set of local variables invisible due to overwrite in deeper namespace[user-set] 
			std::map<unsigned int, dodoStringArr> namespaceVars;///< names of vars in namespaces
			dodoStringArr::iterator x;///< iterator for namespaceVars content
			dodoStringArr::iterator z;///< iterator for namespaceVars content
			std::map<unsigned int, dodoStringArr>::iterator c;///< iterator for namespaceVars 
			std::map<unsigned int, dodoAssocArr>::iterator v;///< iterator for localNamespace
			
			#ifdef FCGI_EXT	
			
				bool cgiFastSet;///< indicates whether cgiFast was set
				
				cgiFastSTD *cf;///< pointer to cgiFast class
				
			#endif 			
	};

};

#endif
