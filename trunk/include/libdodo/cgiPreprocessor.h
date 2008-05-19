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

				dodoString tplBasePath; ///< base path for templates[if empty - full/relative path must be defined in preProcess/include]

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

				dodoArray< dodoArray<unsigned long> > newLinePositions; ///< stack of positions of new lines of templates

				/**
				 * @enum preprocessorStatemetsEnum defines processor statements
				 * @note defines positions of string representation in 'statements' class property
				 */
				enum preprocessorStatemetsEnum
				{
					PREPROCESSOR_PROCESSORSTATEMENT_DODO = 0,
					PREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST,
					PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST,
					PREPROCESSOR_PROCESSORSTATEMENT_OPEN_NP,
					PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NP,
					PREPROCESSOR_PROCESSORSTATEMENT_OPEN_COMM,
					PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_COMM,
					PREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF,
					PREPROCESSOR_PROCESSORSTATEMENT_ELSE,
					PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_IF,
					PREPROCESSOR_PROCESSORSTATEMENT_OPEN_FOR,
					PREPROCESSOR_PROCESSORSTATEMENT_IN,
					PREPROCESSOR_PROCESSORSTATEMENT_KEY_VALUE,
					PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_FOR,
					PREPROCESSOR_PROCESSORSTATEMENT_PRINT,
					PREPROCESSOR_PROCESSORSTATEMENT_BREAK,
					PREPROCESSOR_PROCESSORSTATEMENT_CONT,
					PREPROCESSOR_PROCESSORSTATEMENT_ASSIGN,
					PREPROCESSOR_PREPROCESSOR_PROCESSORSTATEMENT_ASSIGN_OP,
					PREPROCESSOR_PROCESSORSTATEMENT_OPEN_NS,
					PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NS,
					PREPROCESSOR_PREPROCESSOR_PROCESSORSTATEMENT_INCLUDE,
					PREPROCESSOR_PROCESSORSTATEMENT_ITERATOR,
					PREPROCESSOR_PROCESSORSTATEMENT_VERSION,
					PREPROCESSOR_PROCESSORSTATEMENT_DOT,
					PREPROCESSOR_PROCESSORSTATEMENT_COMA,
					PREPROCESSOR_PROCESSORSTATEMENT_DOLLAR,
					PREPROCESSOR_PROCESSORSTATEMENT_FALSE,
					PREPROCESSOR_PROCESSORSTATEMENT_EQ,
					PREPROCESSOR_PROCESSORSTATEMENT_NE,
					PREPROCESSOR_PROCESSORSTATEMENT_LE,
					PREPROCESSOR_PROCESSORSTATEMENT_GE,
					PREPROCESSOR_PROCESSORSTATEMENT_LT,
					PREPROCESSOR_PROCESSORSTATEMENT_GT,
					PREPROCESSOR_PROCESSORSTATEMENT_OPEN_VARPART,
					PREPROCESSOR_PROCESSORSTATEMENT_CLOSE_VARPART,
				};

				static const dodoString statements[36]; ///< processor statement[for dodo.*, ...]
		};
	};
};

#endif
