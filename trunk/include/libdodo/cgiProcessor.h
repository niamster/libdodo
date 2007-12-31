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

#include <libdodo/directives.h>

#include <libdodo/cgiProcessorEx.h>
#include <libdodo/types.h>
#include <libdodo/tools.h>
#include <libdodo/cgiPreprocessor.h>
#include <libdodo/cgiFast.h>
#include <libdodo/ioSTD.h>

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
			cgiProcessor(cgiFastIO *cf);

			#endif

			/**
			 * destructor
			 */
			virtual ~cgiProcessor();

			/**
			 * @return parsed template from file
			 * @param path indicates path where template stays
			 */
			virtual dodoString process(const dodoString &path);

			/**
			 * @return parsed template from buffer
			 * @param tpl indicates template to parse
			 */
			virtual dodoString processString(const dodoString &tpl);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable
			 */
			virtual void assign(dodoString varName, const dodoString &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(array)
			 */
			virtual void assign(dodoString varName, const dodoStringArray &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(hash)
			 */
			virtual void assign(dodoString varName, const dodoStringMap &varVal);

			/**
			 * sets varable
			 * @param varName describes name of variable
			 * @param varVal describes value of variable(array of hashes)
			 */
			virtual void assign(dodoString varName, const dodoArray<dodoStringMap> &varVal);

			/**
			 * show to stdout parsed template
			 * @param path indicates path where template stays
			 */
			virtual void display(const dodoString &path);

			/**
			 * clears internal data[assigned vars, etc.]
			 * @note useful for fastCGI
			 */
			virtual void clear();

		protected:

			/**
			 * @return parsed template from preprocessored buffer
			 * @param path indicates buffer where template stays
			 * @param path indicates file where template got[for reporting]
			 */
			virtual dodoString _processString(const dodoString &buffer, const dodoString &path);

			/**
			 * processes `if` statement
			 * @return position of cursor where to continue search
			 * @param buffer indicates what buffer contains found `if`
			 * @param start indicates where )> closes in `<( if ... )>` block
			 * @param statement indicates `if` statement
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long _if(const dodoString &buffer, unsigned long start, const dodoString &statement, dodoString &tpl, const dodoString &path);

			/**
			 * processes `for` statement
			 * @return position of cursor where to continue search
			 * @param buffer indicates what buffer contains found `for`
			 * @param start indicates where )> closes in `<( for ... )>` block
			 * @param statement indicates `for` statement
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long _for(const dodoString &buffer, unsigned long start, const dodoString &statement, dodoString &tpl, const dodoString &path);

			/**
			 * processes `for` statement
			 * @return position of cursor where to continue search
			 * @param buffer indicates what buffer contains found `for`
			 * @param start indicates where )> closes in `<( namespace ... )>` block
			 * @param tpl indicates string where to add result
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long _ns(const dodoString &buffer, unsigned long start, dodoString &tpl, const dodoString &path);

			/**
			 * processes `print` statement
			 * @param statement indicates `print` statement
			 * @param tpl indicates string where to add result
			 * @param start indicates position in file
			 * @param path indicates path of current .tpl file
			 */
			virtual void _print(const dodoString &statement, dodoString &tpl, unsigned long &start, const dodoString &path);

			/**
			 * processes `break` statement
			 * @param statement indicates `break` statement
			 * @param start indicates position in file
			 * @param path indicates path of current .tpl file
			 */
			virtual bool _break(const dodoString &statement, unsigned long &start, const dodoString &path);

			/**
			 * processes `assign` statement
			 * @param statement indicates `assign` statement
			 * @param start indicates position in file
			 * @param path indicates path of current .tpl file
			 */
			virtual void _assign(const dodoString &statement, unsigned long &start, const dodoString &path);

			/**
			 * processes `include` statement
			 * @param buffer indicates what buffer contains found `if`
			 * @param statement indicates `include` statement
			 * @param tpl indicates string where to add result
			 * @param start indicates position in file
			 * @param path indicates path of current .tpl file
			 */
			virtual void _include(const dodoString &statement, dodoString &tpl, unsigned long &start, const dodoString &path);

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
			 * @param path indicates path of current .tpl file
			 */
			virtual unsigned long blockEnd(const dodoString &buffer, unsigned long start, const dodoString &st, const dodoString &ts, const dodoString &path);

			/**
			 * @return true if path is in `processed` list
			 * @param path desribes what to look up
			 */
			virtual bool recursive(const dodoString &path);

			/**
			 * @return var's value
			 * @param varName describes name of variable
			 * @param start indicates position in file
			 * @param path indicates path of current .tpl file
			 */
			virtual dodoString getVar(const dodoString &varName, unsigned long &start, const dodoString &path);

			/**
			 * @return extracted data(e.g. removes pairs of ",',`)
			 * @param statement describes statement that needs extraction from the pairs of ",',`
			 */
			virtual dodoString trim(const dodoString &statement);

			dodoList<dodoString> processed;                                         ///< vector of files that will be skipped due to recurse

			std::map<dodoString, dodoStringArray> globalArray;                      ///< set of global variables(arrays)[user-set]

			std::map<dodoString, dodoStringMap> globalHash;                         ///< set of global variables(hashes)[user-set]

			std::map<dodoString, dodoArray<dodoStringMap> > globalArrayHash;        ///< set of global variables(array of hashes)[user-set]
			std::map<dodoString, dodoStringMap> localHash;                          ///< set of local variables(hashes)

			dodoStringMap dodo;                                                     ///< set of auxillary variables[dodo defined][for dodo.*]

			dodoStringMap global;                                                   ///< set of global variables[user-set]
			dodoStringMap local;                                                    ///< set of local variables[during parsing]

			bool _continueFlag;                                                     ///< indicates `continue`

			unsigned int _breakDeepness;                                            ///< deepness of the break
			unsigned int _loopDeepness;                                             ///< deepness of the loop

			unsigned long iterator;                                                 ///< count of iteration of a loop

			unsigned int namespaceDeepness;                                         ///< deepness of the namespace
			std::map<unsigned int, dodoStringMap> localNamespace;                   ///< set of local variables invisible due to overwrite in deeper namespace[user-set]
			std::map<unsigned int, dodoStringArray> namespaceVars;                  ///< names of vars in namespaces

			#ifdef FCGI_EXT

			bool cgiFastSet;    ///< indicates whether cgiFast was set

			cgiFastIO *cf;      ///< pointer to cgiFast class

			#endif

			ioSTD *fstd;
	};

};

#endif
