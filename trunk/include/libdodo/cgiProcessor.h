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
#include <libdodo/toolsMisc.h>
#include <libdodo/cgiPreprocessor.h>
#include <libdodo/cgiFastServer.h>
#include <libdodo/cgiServer.h>

namespace dodo
{
	namespace cgi
	{
		/**
		 * @class cgiProcessor parses template
		 */
		class processor : public preprocessor
		{
			public:

				/**
				 * constructor
				 * @param CGI defines instance of cgi object
				 */
				processor(server &CGI);

				/**
				 * destructor
				 */
				virtual ~processor();

				/**
				 * @return parsed template
				 * @param path defines path of template
				 */
				virtual dodoString process(const dodoString &path);

				/**
				 * @return parsed template
				 * @param buffer defines buffer where template is stored
				 */
				virtual dodoString processString(const dodoString &buffer);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variable
				 */
				virtual void assign(dodoString varName, const dodoString &varVal);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variable(array)
				 */
				virtual void assign(dodoString varName, const dodoStringArray &varVal);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variablee(hash)
				 */
				virtual void assign(dodoString varName, const dodoStringMap &varVal);

				/**
				 * set variable
				 * @param varName defines name of variable
				 * @param varVal defines value of variable(array of hashes)
				 */
				virtual void assign(dodoString varName, const dodoArray<dodoStringMap> &varVal);

				/**
				 * pass parsed template to CGI output
				 * @param path indicates path where template stays
				 */
				virtual void display(const dodoString &path);

				/**
				 * clear internal data[assigned vars, etc.]
				 */
				virtual void clear();

			protected:

				/**
				 * @return parsed template from preprocessored buffer
				 * @param buffer defines buffer where template is stored
				 * @param path defines path of template
				 */
				virtual dodoString _processString(const dodoString &buffer, const dodoString &path);

				/**
				 * process `if` statement
				 * @return position of cursor where to continue processing
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of `<( if ... )>` block
				 * @param statement defines `if` statement
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _if(const dodoString &buffer, unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * process `for` statement
				 * @return position of cursor where to continue processing
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of `<( for ... )>` block
				 * @param statement defines `for` statement
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _for(const dodoString &buffer, unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * process `for` statement
				 * @return position of cursor where to continue processing
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of `<( namespace ... )>` block
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _ns(const dodoString &buffer, unsigned long start, dodoString &processed, const dodoString &path);

				/**
				 * process `print` statement
				 * @return position of cursor where to continue processing
				 * @param start defines position in buffer after ')>' of `<( print ... )>` block
				 * @param statement defines `print` statement
				 * @param processed defines buffer where to add processed block
				 * @param path defines path of template
				 */
				virtual unsigned long _print(unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * process `break` statement
				 * @return true if process must break
				 * @param start defines position in buffer after ')>' of `<( break ... )>` block
				 * @param statement defines `break` statement
				 * @param path defines path of template
				 */
				virtual bool _break(unsigned long start, const dodoString &statement, const dodoString &path);

				/**
				 * process `assign` statement
				 * @return position of cursor where to continue processing
				 * @param start defines position in buffer after ')>' of `<( assign ... )>` block
				 * @param statement defines `assign` statement
				 * @param path defines path of template
				 */
				virtual unsigned long _assign(unsigned long start, const dodoString &statement, const dodoString &path);

				/**
				 * process `include` statement
				 * @return position of cursor where to continue processing
				 * @param start defines position in buffer after ')>' of `<( include ... )>` block
				 * @param statement defines `include` statement
				 * @param processed indicates string where to add result
				 * @param path defines path of template
				 */
				virtual unsigned long _include(unsigned long start, const dodoString &statement, dodoString &processed, const dodoString &path);

				/**
				 * clean namespace variable and bring back to life vars of prevous namespace that were overwritten
				 */
				virtual void cleanNamespace();

				/**
				 * @return position in buffer after ')>' of the end block
				 * @param buffer defines buffer where template is stored
				 * @param start defines position in buffer after ')>' of the block
				 * @param st defines open statement[if, for ...]
				 * @param ts defines close statement[fi, rof ...]
				 * @param path defines path of template
				 */
				virtual unsigned long blockEnd(const dodoString &buffer, unsigned long start, const dodoString &st, const dodoString &ts, const dodoString &path);

				/**
				 * @return true if path is in `processed` list
				 * @param path defines path that might cause resursive include
				 */
				virtual bool recursive(const dodoString &path);

				/**
				 * @return value of the variable
				 * @param varName defines name of variable
				 * @param start defines position in file
				 * @param path defines path of template
				 */
				virtual dodoString getVar(const dodoString &varName, unsigned long start, const dodoString &path);

				/**
				 * @return extracted data(e.g. removes pairs of ",',`)
				 * @param statement defines statement that needs extraction from the pairs of ",',`
				 */
				virtual dodoString trim(const dodoString &statement);

				dodoList<dodoString> processed;                                             ///< files that will be skipped due to the recursion

				dodoStringMap global;                                                       ///< set of global variables[user-set]
				dodoMap<dodoString, dodoStringArray> globalArray;                           ///< global user-defined variables(arrays)
				dodoMap<dodoString, dodoStringMap> globalHash;                              ///< global user-defined variables(hashes)
				dodoMap<dodoString, dodoArray<dodoStringMap> > globalArrayHash;             ///< global user-defined variables(array of hashes)

				dodoStringMap local;                                                        ///< local variables[occur in `for` block]
				dodoMap<dodoString, dodoStringMap> localHash;                               ///< local variables(hashes)[occur in `for` block]

				dodoStringMap dodo;                                                         ///< auxillary variables[dodo defined][for dodo.*]

				bool continueFlag;                                                          ///< indicates `continue` state

				unsigned int breakDeepness;                                                 ///< deepness of the break
				unsigned int loopDeepness;                                                  ///< deepness of the loop

				unsigned long iterator;                                                     ///< amount of iterations of the loop

				unsigned int namespaceDeepness;                                             ///< deepness of the namespace
				dodoMap<unsigned int, dodoStringMap> localNamespace;                        ///< local variables invisible due to overwrite in deeper namespace
				dodoMap<unsigned int, dodoStringArray> namespaceVars;                       ///< names of vars in namespaces

				server &CGI;                                                                ///< cgi object through what output will be performed
		};
	};
};

#endif
