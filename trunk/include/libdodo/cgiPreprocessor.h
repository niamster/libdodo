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

#include <libdodo/tools.h>
#include <libdodo/cgiPreprocessorEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsFilesystem.h>

namespace dodo
{
	/**
	 * @class cgiPreprocessor prepares template for cgiProcessor
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
				CGIPREPROCESSOR_PROCESSORSTATEMENT_DODO = 0,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_ST,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_ST,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_NP,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NP,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_COMM,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_COMM,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_IF,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_ELSE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_IF,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_FOR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_IN,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_KEY_VALUE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_FOR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_PRINT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_BREAK,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CONT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_ASSIGN,
				CGIPREPROCESSOR_CGIPREPROCESSOR_PROCESSORSTATEMENT_ASSIGN_OP,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_NS,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_NS,
				CGIPREPROCESSOR_CGIPREPROCESSOR_PROCESSORSTATEMENT_INCLUDE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_ITERATOR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_VERSION,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_DOT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_COMA,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_DOLLAR,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_FALSE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_EQ,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_NE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_LE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_GE,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_LT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_GT,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_OPEN_VARPART,
				CGIPREPROCESSOR_PROCESSORSTATEMENT_CLOSE_VARPART,
			};
			
			static const dodoString statements[36]; ///< processor statement[for dodo.*, ...]
	};
};

#endif
