/***************************************************************************
 *            ioPipeEx.h
 *
 *  Tue Jul 1 15:34:57 2008
 *  Copyright  2008  Ni@m
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

#ifndef _IOPIPEEX_H_
#define _IOPIPEEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace io {
		/**
		 * libdodo defined errors
		 */
		enum pipeExR {
			PIPEEX_PIPENOTOPENED,
		};

		/**
		 * explanations for libdodo defined errors
		 */
#define IOPIPEEX_NOTOPENED_STR                  "PIPE was not opened or assigned."

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum pipeFunctionsID {
			PIPEEX_PIPE,
			PIPEEX_CLOSE,
			PIPEEX_CLONE,
			PIPEEX_OPEN,
			PIPEEX__READ,
			PIPEEX__WRITE,
			PIPEEX__READSTREAM,
			PIPEEX__WRITESTREAM,
			PIPEEX_FLUSH,
			PIPEEX_PEERINFO,
			PIPEEX_BLOCK,
			PIPEEX_GETINDESCRIPTOR,
			PIPEEX_GETOUTDESCRIPTOR,
		};
	};
};

#endif
