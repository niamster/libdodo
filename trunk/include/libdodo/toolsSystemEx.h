/***************************************************************************
 *            toolsSystem.h
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
	enum toolsSystemExR
	{
		TOOLSSYSTEMEX_WRONGPARAMETER,
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define TOOLSSYSTEMEX_WRONGPARAMETER_STR "Wrong paramather passed to function."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum toolsSystemFunctionsID
	{
		TOOLSSYSTEMSYSTEMATOMICMUTEXEX_UNLOCK,
		TOOLSSYSTEMSYSTEMATOMICMUTEXEX_LOCK,
		TOOLSSYSTEMEX_SETWORKINGDIR,
		TOOLSSYSTEMEX_GETWORKINGDIR,
		TOOLSSYSTEMEX_GETUSAGEINFO,
		TOOLSSYSTEMEX_GETLIMIT,
		TOOLSSYSTEMEX_SETLIMIT,
		TOOLSSYSTEMEX_GETPRIORITY,
		TOOLSSYSTEMEX_SETPRIORITY,
		TOOLSSYSTEMEX_GETUID,
		TOOLSSYSTEMEX_SETUID,
		TOOLSSYSTEMEX_GETGID,
		TOOLSSYSTEMEX_SETGID,
		TOOLSSYSTEMEX_GETUSERINFO,
		TOOLSSYSTEMEX_GETUSERS,
		TOOLSSYSTEMEX_GETGROUPINFO,
		TOOLSSYSTEMEX_GETGROUPS,
		TOOLSSYSTEMEX_CHANGEROOT,
		TOOLSSYSTEMEX_ATEXIT,
		TOOLSSYSTEMEX_GETGROUPPID,
		TOOLSSYSTEMEX_SETGROUPPID,
		TOOLSSYSTEMEX_SETSIGNALHANDLER,
		TOOLSSYSTEMEX_UNSETSIGNALHANDLER,
		TOOLSSYSTEMEX_GETMODULEINFO,
		TOOLSSYSTEMEX_SENDSIGNAL,
		TOOLSSYSTEMEX_SETTIMER,
		TOOLSSYSTEMEX_SETMICROTIMER,
		TOOLSSYSTEMEX_DAEMONIZE,
	};
};

#endif
