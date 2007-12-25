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
#include <libdodo/ioDiskTools.h>

namespace dodo
{

	/**
	 * @class cgiPreprocessor prepares template for future parsings
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
			virtual dodoString preProcess(const dodoString &path);

			/**
			 * @return preprocessed template from buffer
			 * @param tpl indicates template to preprocess
			 */
			virtual dodoString preProcessString(const dodoString &tpl);

		protected:

			/**
			 * @return preprocessed template from buffer
			 * @param tpl indicates template to preprocess
			 * @param path indicates path where template stays[for reporting]
			 */
			virtual dodoString _preProcessString(const dodoString &tpl, const dodoString &path);

			/**
			 * @return line number in file
			 * @param newLinePos describes array of newlines' positions
			 * @param pos describes pos in file
			 */
			virtual unsigned long getLineNumber(const dodoArray<unsigned long> &newLinePos, unsigned long pos);

			/**
			 * @return newlines positions
			 * @param tpl indicates template to process
			 */
			virtual dodoArray<unsigned long> detectNewLines(const dodoString &tpl);

			dodoArray< dodoArray<unsigned long> > newLinePositions; ///< stack of processed new lines' positions

			/**
			 * @enum processorStatemetsEnum describes positions in 'statements' property
			 */
			enum processorStatemetsEnum
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
			static const dodoString statements[36]; ///< dodo statement[for dodo.*, ...]
	};

};

#endif
