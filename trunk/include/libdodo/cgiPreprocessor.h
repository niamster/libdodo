/***************************************************************************
 *            cgiPreprocessor.h
 *
 *  Sun Jan 22 19:05:57 2006
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

#ifndef _CGIPREPROCESSOR_H_
#define _CGIPREPROCESSOR_H_

#include <libdodo/directives.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libdodo/toolsMisc.h>
#include <libdodo/cgiPreprocessorEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsFilesystem.h>

namespace dodo
{
	namespace cgi
	{
		/**
		 * @class preprocessor prepares template for cgi::processor
		 */
		class preprocessor
		{
			public:

				/**
				 * constructor
				 */
				preprocessor();

				/**
				 * destructor
				 */
				virtual ~preprocessor();

				/**
				 * @return preprocessed template
				 * @param path defines path of template
				 */
				virtual dodoString preProcess(const dodoString &path);

				/**
				 * @return preprocessed template
				 * @param buffer defines buffer where template is stored
				 */
				virtual dodoString preProcessString(const dodoString &buffer);

				dodoString tplBasePath;                 ///< base path for templates[if empty - full/relative path must be defined in preProcess/include]

			protected:

				/**
				 * @return preprocessed template
				 * @param buffer defines buffer where template is stored
				 * @param path defines path of template
				 */
				virtual dodoString _preProcessString(const dodoString &buffer, const dodoString &path);

				/**
				 * @return line number in file
				 * @param newlines defines positions of new lines if template
				 * @param pos defines position of symbol in template
				 */
				virtual unsigned long getLineNumber(const dodoArray<unsigned long> &newlines, unsigned long pos);

				/**
				 * @return positions of new lines
				 * @param buffer defines buffer where template is stored
				 */
				virtual dodoArray<unsigned long> detectNewLines(const dodoString &buffer);

				dodoArray< dodoArray<unsigned long> > newLinePositions;                 ///< stack of positions of new lines of templates

#define PREPROCESSOR_STATEMENTS 30

				/**
				 * @enum preprocessorStatementEnum defines processor statements
				 * @note defines positions of string representation in 'statements' class property
				 */
				enum preprocessorStatementEnum
				{
					PREPROCESSOR_STATEMENT_DODO = 0,
					PREPROCESSOR_STATEMENT_OPEN_ST,
					PREPROCESSOR_STATEMENT_CLOSE_ST,
					PREPROCESSOR_STATEMENT_OPEN_NP,
					PREPROCESSOR_STATEMENT_CLOSE_NP,
					PREPROCESSOR_STATEMENT_OPEN_COMM,
					PREPROCESSOR_STATEMENT_CLOSE_COMM,
					PREPROCESSOR_STATEMENT_OPEN_IF,
					PREPROCESSOR_STATEMENT_ELSE,
					PREPROCESSOR_STATEMENT_CLOSE_IF,
					PREPROCESSOR_STATEMENT_OPEN_FOR,
					PREPROCESSOR_STATEMENT_IN,
					PREPROCESSOR_STATEMENT_KEY_VALUE,
					PREPROCESSOR_STATEMENT_CLOSE_FOR,
					PREPROCESSOR_STATEMENT_PRINT,
					PREPROCESSOR_STATEMENT_BREAK,
					PREPROCESSOR_STATEMENT_CONT,
					PREPROCESSOR_STATEMENT_ASSIGN,
					PREPROCESSOR_STATEMENT_ASSIGN_OP,
					PREPROCESSOR_STATEMENT_OPEN_NS,
					PREPROCESSOR_STATEMENT_CLOSE_NS,
					PREPROCESSOR_STATEMENT_INCLUDE,
					PREPROCESSOR_STATEMENT_ITERATOR,
					PREPROCESSOR_STATEMENT_VERSION,
					PREPROCESSOR_STATEMENT_DOT,
					PREPROCESSOR_STATEMENT_COMA,
					PREPROCESSOR_STATEMENT_DOLLAR,
					PREPROCESSOR_STATEMENT_FALSE,
					PREPROCESSOR_STATEMENT_OPEN_VARPART,
					PREPROCESSOR_STATEMENT_CLOSE_VARPART,
				};

				static const dodoString statements[PREPROCESSOR_STATEMENTS];                 ///< processor statements[for dodo.*, ...]
		};
	};
};

#endif
