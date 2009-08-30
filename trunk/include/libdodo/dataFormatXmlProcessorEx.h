/***************************************************************************
 *            dataFormatXmlProcessorEx.h
 *
 *  Wed Oct 5 16:25:14 2005
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DATAFORMATXMLPROCESSOREX_H_
#define _DATAFORMATXMLPROCESSOREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace data {
		namespace format {
			namespace xml {
				/**
				 * libdodo defined errors
				 */
				enum processorExR {
					PROCESSOREX_EMPTYDOCUMENT,
					PROCESSOREX_NOROOTNODE,
					PROCESSOREX_NONAME,
				};

				/**
				 * explanations for libdodo defined errors
				 */
#define DATAFORMATXMLPROCESSOREX_EMPTYDOCUMENT_STR "File or buffer doesn't contain XML document"
#define DATAFORMATXMLPROCESSOREX_NOROOTNODE_STR    "XML document doesn't have root node"
#define DATAFORMATXMLPROCESSOREX_NONAME_STR    "Node does not have any name"

				/**
				 * IDs of functions where exception might be thrown
				 */
				enum processorFunctionsID {
					PROCESSOREX_PARSEFILE,
					PROCESSOREX_PARSE,
					PROCESSOREX_ERRHANDLER,
					PROCESSOREX_GETBUFFERINFO,
					PROCESSOREX_PARSEBUFFER,
					PROCESSOREX_GETFILEINFO,
					PROCESSOREX_PARSEBUFFERINT,
					PROCESSOREX_PARSEFILEINT,
					PROCESSOREX_MAKE,
				};
			};
		};
	};
};
#endif

