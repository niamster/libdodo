/***************************************************************************
 *            toolsFilesystemEx.h
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

#ifndef _TOOLSFILESYSTEMEX_H_
#define _TOOLSFILESYSTEMEX_H_

#include <libdodo/directives.h>

#include <libdodo/baseEx.h>

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum toolsFilesystemExR
	{
		TOOLSFILESYSTEMEX_WRONGFILENAME,
		TOOLSFILESYSTEMEX_DESTINATIONEXISTS,
		TOOLSFILESYSTEMEX_NOTADIR,
		TOOLSFILESYSTEMEX_TOOLONGPATH
	};

	/**
	 * explanations for libdodo defined errors
	 */
#define TOOLSFILESYSTEMEX_WRONGFILENAME_STR "Probably wrong filename, type of file is wrong or `path` is empty!"
#define TOOLSFILESYSTEMEX_DESTINATIONEXISTS_STR "Destination file/dir/etc. exists. Won't overvrite"
#define TOOLSFILESYSTEMEX_NOTADIR_STR "There is not a dir with given path."
#define TOOLSFILESYSTEMEX_TOOLONGPATH_STR "Too long path."

	/**
	 * IDs of functions where exception might be thrown
	 */
	enum toolsFilesystemFunctionsID
	{
		TOOLSFILESYSTEMEX_UNLINK,
		TOOLSFILESYSTEMEX_RENAME,
		TOOLSFILESYSTEMEX_LINK,
		TOOLSFILESYSTEMEX_SYMLINK,
		TOOLSFILESYSTEMEX_CHOWN,
		TOOLSFILESYSTEMEX_CHGRP,
		TOOLSFILESYSTEMEX_GETUSEROWNER,
		TOOLSFILESYSTEMEX_GETGROUPOWNER,
		TOOLSFILESYSTEMEX_TOUCH,
		TOOLSFILESYSTEMEX_MKDIR,
		TOOLSFILESYSTEMEX_MKDIRRECURSIVE,
		TOOLSFILESYSTEMEX_MKFIFO,
		TOOLSFILESYSTEMEX_CHMOD,
		TOOLSFILESYSTEMEX_RM,
		TOOLSFILESYSTEMEX_GETPERMISSIONS,
		TOOLSFILESYSTEMEX_GETFILETYPE,
		TOOLSFILESYSTEMEX_GETSIZE,
		TOOLSFILESYSTEMEX_GETACCTIME,
		TOOLSFILESYSTEMEX_GETMODTIME,
		TOOLSFILESYSTEMEX_GETFILEINFO,
		TOOLSFILESYSTEMEX_GETDIRINFO,
		TOOLSFILESYSTEMEX_FOLLOWSYMLINK,
		TOOLSFILESYSTEMEX_GETFILECONTENTS,
		TOOLSFILESYSTEMEX_GETFILECONTENTSARR,
		TOOLSFILESYSTEMEX_COPY,
		TOOLSFILESYSTEMEX_COPYDIR,
		TOOLSFILESYSTEMEX__WRITETOFILE,
		TOOLSFILESYSTEMEX_DIRNAME,
		TOOLSFILESYSTEMEX_LASTNAME,
	};
};

#endif
