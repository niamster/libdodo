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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _TOOLSFILESYSTEMEX_H_
#define _TOOLSFILESYSTEMEX_H_ 1

#include <libdodo/directives.h>

#include <libdodo/exceptionBasic.h>

namespace dodo {
	namespace tools {
		/**
		 * libdodo defined errors
		 */
		enum toolsFilesystemExR {
			FILESYSTEMEX_WRONGFILENAME,
			FILESYSTEMEX_DESTINATIONEXISTS,
			FILESYSTEMEX_NOTADIR,
			FILESYSTEMEX_TOOLONGPATH
		};

		/**
		 * explanations for libdodo defined errors
		 */
#define TOOLSFILESYSTEMEX_WRONGFILENAME_STR     "Wrong filename"
#define TOOLSFILESYSTEMEX_DESTINATIONEXISTS_STR "Destination file/dir/etc. exists"
#define TOOLSFILESYSTEMEX_NOTADIR_STR           "There is not a dir with given path"
#define TOOLSFILESYSTEMEX_TOOLONGPATH_STR       "Path is too long"

		/**
		 * IDs of functions where exception might be thrown
		 */
		enum filesystemFunctionsID {
			FILESYSTEMEX_UNLINK,
			FILESYSTEMEX_RENAME,
			FILESYSTEMEX_LINK,
			FILESYSTEMEX_SYMLINK,
			FILESYSTEMEX_CHOWN,
			FILESYSTEMEX_CHGRP,
			FILESYSTEMEX_USEROWNER,
			FILESYSTEMEX_GROUPOWNER,
			FILESYSTEMEX_TOUCH,
			FILESYSTEMEX_MKDIR,
			FILESYSTEMEX_MKFIFO,
			FILESYSTEMEX_CHMOD,
			FILESYSTEMEX_RM,
			FILESYSTEMEX_PERMISSIONS,
			FILESYSTEMEX_FILETYPE,
			FILESYSTEMEX_SIZE,
			FILESYSTEMEX_ACCTIME,
			FILESYSTEMEX_MODTIME,
			FILESYSTEMEX_FILEINFO,
			FILESYSTEMEX_DIRINFO,
			FILESYSTEMEX_FOLLOWSYMLINK,
			FILESYSTEMEX_FILECONTENTS,
			FILESYSTEMEX_FILECONTENTSARR,
			FILESYSTEMEX_COPY,
			FILESYSTEMEX_COPYDIR,
			FILESYSTEMEX__WRITETOFILE,
			FILESYSTEMEX_DIRNAME,
			FILESYSTEMEX_BASENAME,
		};
	};
};
#endif
