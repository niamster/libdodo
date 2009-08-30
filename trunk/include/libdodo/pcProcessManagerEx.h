/***************************************************************************
 *            pcProcessManagerEx.h
 *
 *  Tue Feb 27 08:42:14 2007
 *  Copyright  2007  Ni@m
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

#ifndef _PCPROCESSMANAGEREX_H_
#define _PCPROCESSMANAGEREX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace pc {
		namespace process {
			/**
			 * libdodo defined errors
			 */
			enum managerExR {
				MANAGEREX_ISALREADYRUNNING,
				MANAGEREX_NOTFOUND,
			};

			/**
			 * explanations for libdodo defined errors
			 */
#define PCPROCESSMANAGEREX_ISALREADYRUNNING_STR "Process is currently running"
#define PCPROCESSMANAGEREX_NOTFOUND_STR         "Process not found"

			/**
			 * IDs of functions where exception might be thrown
			 */
			enum managerFunctionsID {
				MANAGEREX__ISRUNNING,
				MANAGEREX_ADDNRUN,
				MANAGEREX_RUN,
				MANAGEREX_REMOVE,
				MANAGEREX_STOP,
				MANAGEREX_WAIT,
				MANAGEREX_ISRUNNING,
#ifdef DL_EXT
				MANAGEREX_MODULE,
				MANAGEREX_ADD,
#endif
			};
		};
	};
};
#endif

