/***************************************************************************
 *            toolsOsEx.h
 *
 *  Sat Nov 19 15:19:57 2005
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

#ifndef _TOOLSOSEX_H_
#define _TOOLSOSEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{
	namespace tools
	{
		/**
		 * libdodo defined errors
		 */
		enum osExR
		{
			OSEX_WRONGPARAMETER,
		};

		/**
		 * explanations for libdodo defined errors
		 */
#define TOOLSOSEX_WRONGPARAMETER_STR "Wrong paramather passed to function."

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum osFunctionsID
		{
			SYSTEMSTATICATOMICMUTEXEX_UNLOCK,
			SYSTEMSTATICATOMICMUTEXEX_LOCK,
			OSEX_SETWORKINGDIR,
			OSEX_GETWORKINGDIR,
			OSEX_GETUSAGEINFO,
			OSEX_GETLIMIT,
			OSEX_SETLIMIT,
			OSEX_GETPRIORITY,
			OSEX_SETPRIORITY,
			OSEX_GETUID,
			OSEX_SETUID,
			OSEX_GETGID,
			OSEX_SETGID,
			OSEX_GETUSERINFO,
			OSEX_GETUSERS,
			OSEX_GETGROUPINFO,
			OSEX_GETGROUPS,
			OSEX_CHANGEROOT,
			OSEX_ATEXIT,
			OSEX_GETGROUPPID,
			OSEX_SETGROUPPID,
			OSEX_SETSIGNALHANDLER,
			OSEX_UNSETSIGNALHANDLER,
			OSEX_GETMODULEINFO,
			OSEX_SENDSIGNAL,
			OSEX_SETTIMER,
			OSEX_SETMICROTIMER,
			OSEX_DAEMONIZE,
		};
	};
};

#endif
