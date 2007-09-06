/***************************************************************************
 *            flushDiskToolsEx.h
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

#ifndef _FLUSHDISKTOOLSEX_H_
#define _FLUSHDISKTOOLSEX_H_

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

namespace dodo
{

	/**
	 * libdodo defined errors
	 */
	enum flushDiskToolsExR
	{
		FLUSHDISKTOOLS_WRONG_FILENAME,
		FLUSHDISKTOOLS_DESTINATION_EXISTS,
	};

	/**
	 * libdodo defined errors' explanation
	 */
        #define FLUSHDISKTOOLS_WRONG_FILENAME_STR        "Probably wrong filename, type of file is wrong or `path` is empty!"
        #define FLUSHDISKTOOLS_DESTINATION_EXISTS_STR    "Destination file/dir/etc. exists. Won't overvrite"

	/**
	 * ID of function where exception was thrown
	 */
	enum flushDiskToolsFunctionsID
	{
		FLUSHDISKTOOLS_UNLINK,
		FLUSHDISKTOOLS_RENAME,
		FLUSHDISKTOOLS_LINK,
		FLUSHDISKTOOLS_SYMLINK,
		FLUSHDISKTOOLS_CHOWN,
		FLUSHDISKTOOLS_CHGRP,
		FLUSHDISKTOOLS_GETUSEROWNER,
		FLUSHDISKTOOLS_GETGROUPOWNER,
		FLUSHDISKTOOLS_TOUCH,
		FLUSHDISKTOOLS_MKDIR,
		FLUSHDISKTOOLS_CHMOD,
		FLUSHDISKTOOLS_RM,
		FLUSHDISKTOOLS_GETPERMISSIONS,
		FLUSHDISKTOOLS_GETFILETYPE,
		FLUSHDISKTOOLS_GETSIZE,
		FLUSHDISKTOOLS_GETACCTIME,
		FLUSHDISKTOOLS_GETMODTIME,
		FLUSHDISKTOOLS_GETFILEINFO,
		FLUSHDISKTOOLS_GETDIRINFO,
		FLUSHDISKTOOLS_FOLLOWSYMLINK,
		FLUSHDISKTOOLS_GETFILECONTENT,
		FLUSHDISKTOOLS_GETFILECONTENTARR,
		FLUSHDISKTOOLS_COPY,
		FLUSHDISKTOOLS_COPYDIR,
		FLUSHDISKTOOLS_APPEND,
	};
};

#endif

#endif
