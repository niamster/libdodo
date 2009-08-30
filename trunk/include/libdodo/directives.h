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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DIRECTIVES_H_
#define _DIRECTIVES_H_ 1

#include <libdodo/directives.runtime.h>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef __USE___UNIX98
#define __USE___UNIX98
#endif

#ifndef _REENTRANT
#define _REENTRANT
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

/**
 * size of key for shm_open and sem_open
 * @note including leading '/'
 */
#ifdef __FreeBSD__
#define SH_KEY_SIZE 14
#else
#define SH_KEY_SIZE 32
#endif

/**
 * size of block for zLib de/compression
 */
#define ZLIB_CHUNK    100

/**
 * size of block for bzip2 de/compression
 */
#define BZIP_CHUNK    100

/**
 * file permissions for newly created file(pipe)
 */
#define DEFAULT_FILE_PERM dodo::tools::filesystem::PERMISSION_OWNER_ALL_ACCESS

/**
 * size of buffer for random data for openssl library
 */
#define OPENSSL_RAND_BUFSIZE 512

/**
 * size of string that represents numeric value during convertion
 */
#define SIZEOFNUM     20

/**
 * socket default timeouts
 */
#define IONETWORKCONNECTION_SOCKET_RECIEVE_TIMEOUT          1000
#define IONETWORKCONNECTION_SOCKET_SEND_TIMEOUT             1000

/**
 * default values of buffer for io operations(io*)
 */
#define IO_INSIZE                   4096
#define IO_OUTSIZE                  4096

/**
 * max line length
 */
#define PATH_MAXLINELEN          10240

/**
 * default values of socket buffer for io operations()
 */
#define IONETWORKCONNECTION_SOCKET_INSIZE            102400
#define IONETWORKCONNECTION_SOCKET_OUTSIZE           102400

/**
 * default values of stdin/out buffer for io operations(stdio)
 */
#define IOPIPE_INSIZE               102400
#define IOPIPE_OUTSIZE              102400

/**
 * default values of stdin/out buffer for io operations(stdio)
 */
#define IOFILEFIFO_INSIZE               102400
#define IOFILEFIFO_OUTSIZE              102400

/**
 * linger options
 */
#define IONETWORKCONNECTION_SOCKET_LINGER_PERIOD     1
#define IONETWORKCONNECTION_SOCKET_LINGER_OPTION     LINGER_OPTION_WAIT_CLOSE

/**
 * max length of the filesystem path
 */
#define MAXPATHLEN               FILENAME_MAX

/**
 * max depth of calls for stack trace
 */
#define MAXCALLSTACKLEN 128
#endif
