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
		IODISKTOOLS_WRONG_FILENAME,
		IODISKTOOLS_DESTINATION_EXISTS,
	};

	/**
	 * libdodo defined errors' explanation
	 */
		#define IODISKTOOLS_WRONG_FILENAME_STR        "Probably wrong filename, type of file is wrong or `path` is empty!"
		#define IODISKTOOLS_DESTINATION_EXISTS_STR    "Destination file/dir/etc. exists. Won't overvrite"

	/**
	 * ID of function where exception was thrown
	 */
	enum ioDiskToolsFunctionsID
	{
		IODISKTOOLS_UNLINK,
		IODISKTOOLS_RENAME,
		IODISKTOOLS_LINK,
		IODISKTOOLS_SYMLINK,
		IODISKTOOLS_CHOWN,
		IODISKTOOLS_CHGRP,
		IODISKTOOLS_GETUSEROWNER,
		IODISKTOOLS_GETGROUPOWNER,
		IODISKTOOLS_TOUCH,
		IODISKTOOLS_MKDIR,
		IODISKTOOLS_CHMOD,
		IODISKTOOLS_RM,
		IODISKTOOLS_GETPERMISSIONS,
		IODISKTOOLS_GETFILETYPE,
		IODISKTOOLS_GETSIZE,
		IODISKTOOLS_GETACCTIME,
		IODISKTOOLS_GETMODTIME,
		IODISKTOOLS_GETFILEINFO,
		IODISKTOOLS_GETDIRINFO,
		IODISKTOOLS_FOLLOWSYMLINK,
		IODISKTOOLS_GETFILECONTENT,
		IODISKTOOLS_GETFILECONTENTARR,
		IODISKTOOLS_COPY,
		IODISKTOOLS_COPYDIR,
		IODISKTOOLS_APPEND,
	};
};

#endif
