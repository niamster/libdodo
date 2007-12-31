/***************************************************************************
 *            systemTools.h
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

#ifndef _SYSTEMEX_H_
#define _SYSTEMEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum systemToolsExR
	{
		SYSTEMTOOLSEX_WRONGPARAMETER,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define SYSTEMTOOLSEX_WRONGPARAMETER_STR    "Wrong paramather passed to function."

	/**
	 * ID of function where exception was thrown
	 */
	enum systemToolsFunctionsID
	{
		SYSTEMTOOLSEX_SETWORKINGDIR,
		SYSTEMTOOLSEX_GETWORKINGDIR,
		SYSTEMTOOLSEX_GETUSAGEINFO,
		SYSTEMTOOLSEX_GETLIMIT,
		SYSTEMTOOLSEX_SETLIMIT,
		SYSTEMTOOLSEX_GETPRIORITY,
		SYSTEMTOOLSEX_SETPRIORITY,
		SYSTEMTOOLSEX_GETUID,
		SYSTEMTOOLSEX_SETUID,
		SYSTEMTOOLSEX_GETGID,
		SYSTEMTOOLSEX_SETGID,
		SYSTEMTOOLSEX_GETUSERINFO,
		SYSTEMTOOLSEX_GETUSERS,
		SYSTEMTOOLSEX_GETGROUPINFO,
		SYSTEMTOOLSEX_GETGROUPS,
		SYSTEMTOOLSEX_CHANGEROOT,
		SYSTEMTOOLSEX_ATEXIT,
		SYSTEMTOOLSEX_GETGROUPPID,
		SYSTEMTOOLSEX_SETGROUPPID,
		SYSTEMTOOLSEX_SETSIGNALHANDLER,
		SYSTEMTOOLSEX_UNSETSIGNALHANDLER,
		SYSTEMTOOLSEX_GETMODULEINFO,
		SYSTEMTOOLSEX_SENDSIGNAL,
		SYSTEMTOOLSEX_SETTIMER,
		SYSTEMTOOLSEX_SETMICROTIMER,
		SYSTEMTOOLSEX_DAEMONIZE,
	};
};

#endif
