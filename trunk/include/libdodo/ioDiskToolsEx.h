/***************************************************************************
 *            ioDiskToolsEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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

#ifndef _IODISKTOOLSEX_H_
#define _IODISKTOOLSEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum ioDiskToolsExR
	{
		IODISKTOOLSEX_WRONGFILENAME,
		IODISKTOOLSEX_DESTINATIONEXISTS,
		IODISKTOOLSEX_NOTADIR,
		IODISKTOOLSEX_TOOLONGPATH
	};

	/**
	 * libdodo defined errors' explanation
	 */
#define IODISKTOOLSEX_WRONGFILENAME_STR "Probably wrong filename, type of file is wrong or `path` is empty!"
#define IODISKTOOLSEX_DESTINATIONEXISTS_STR "Destination file/dir/etc. exists. Won't overvrite"
#define IODISKTOOLSEX_NOTADIR_STR "There is not a dir with given path."
#define IODISKTOOLSEX_TOOLONGPATH_STR "Too long path."

	/**
	 * ID of function where exception was thrown
	 */
	enum ioDiskToolsFunctionsID
	{
		IODISKTOOLSEX_UNLINK,
		IODISKTOOLSEX_RENAME,
		IODISKTOOLSEX_LINK,
		IODISKTOOLSEX_SYMLINK,
		IODISKTOOLSEX_CHOWN,
		IODISKTOOLSEX_CHGRP,
		IODISKTOOLSEX_GETUSEROWNER,
		IODISKTOOLSEX_GETGROUPOWNER,
		IODISKTOOLSEX_TOUCH,
		IODISKTOOLSEX_MKDIR,
		IODISKTOOLSEX_MKDIRRECURSIVE,
		IODISKTOOLSEX_MKFIFO,
		IODISKTOOLSEX_CHMOD,
		IODISKTOOLSEX_RM,
		IODISKTOOLSEX_GETPERMISSIONS,
		IODISKTOOLSEX_GETFILETYPE,
		IODISKTOOLSEX_GETSIZE,
		IODISKTOOLSEX_GETACCTIME,
		IODISKTOOLSEX_GETMODTIME,
		IODISKTOOLSEX_GETFILEINFO,
		IODISKTOOLSEX_GETDIRINFO,
		IODISKTOOLSEX_FOLLOWSYMLINK,
		IODISKTOOLSEX_GETFILECONTENT,
		IODISKTOOLSEX_GETFILECONTENTARR,
		IODISKTOOLSEX_COPY,
		IODISKTOOLSEX_COPYDIR,
		IODISKTOOLSEX_APPEND,
		IODISKTOOLSEX_DIRNAME,
		IODISKTOOLSEX_LASTNAME,
	};
};

#endif
