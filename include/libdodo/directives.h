/***************************************************************************
 *            directives.h
 *
 *  Fri Jul 15 22:19:48 2005
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
 
#ifndef _DIRECTIVES_H_
#define _DIRECTIVES_H_

#include <directives.runtime.h>

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#ifndef __USE_UNIX98
	#define __USE_UNIX98
#endif

#ifndef _REENTRANT
	#define _REENTRANT
#endif

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <unistd.h>

/**
 * other options
 */

/**
 * !!!!note!!!!!
 * please read notes before uncommentin'!!!
 */

/**
 * size of block to use with zLib de/compression
 */
#define ZLIB_CHUNK 100

#define BZIP_CHUNK 100

/**
 *  describes file permissions for newly created file(pipe) with open method
 */
#define FILE_PERM (PERM_OWNER_READ_ACCESS|PERM_OWNER_WRITE_ACCESS|PERM_OWNER_EXECUTE_ACCESS)

/**
 * is used for converting from number to string
 */
#define SIZEOFNUM 20

/**
 * delimiter in disk operations
 */
#define FILE_DELIM '/'

/**
 * removes some checks
 * 1) check for request completeness(sqlBase)
 */
#define FAST

/**
 * 
 */
//#define CGI_SAVEPROCESS

/**
 * use modules without hooks. faster a little bit.
 */
//#define FLUSH_DISK_WO_XEXEC=a little bit faster, if u don't want to use it!
//#define DBMYSQL_WO_XEXEC
//#define DBSQLITE_WO_XEXEC
//#define FLUSH_STD_WO_XEXEC
//#define FLUSH_SOCKET_WO_XEXEC
//#define FLUSH_SOCKETEXCHANGE_WO_XEXEC
//#define DBPOSTGRESQL_WO_XEXEC

/**
 * socket default timeouts
 */
#define RECIEVE_TIMEOUT 1000
#define SEND_TIMEOUT 1000

/**
 * for flushDisk::getFileContentArr
 */
#define DISK_MAXLINELEN 10240

/**
 * size of batch for POST reading in cgiTool
 */
#define POST_BATCH_SIZE 4096

/**
 * default values of buffer for io operations(flush*)
 */
#define INSIZE 4096
#define OUTSIZE 4096	

/**
 * default values of socket buffer for io operations(flushSocket)
 */
#define SOCKET_INSIZE 10240
#define SOCKET_OUTSIZE 10240

/**
 * default values of stdin/out buffer for io operations(flushSTD)
 */
#define STD_INSIZE 1024000
#define STD_OUTSIZE 1024000

/**
 * amount of seconds for linger
 */
#define SOCKET_LINGER_PERIOD 1
#define SOCKET_LINGER_OPTION SOCKET_WAIT_CLOSE

#define TOOLS_SHORT_DATA_SIZE 2048

/**
 * max length of the file path
 */
#define MAXPATHLEN FILENAME_MAX

#endif
