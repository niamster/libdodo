/***************************************************************************
 *			  directives.h
 *
 *	Fri Jul 15 2005
 *	Copyright  2005	 Dmytro Milinevskyy
 *	milinevskyy@gmail.com
 ****************************************************************************/

/*
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *	the Free Software Foundation;
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU Library General Public License for more details.
 *
 *	You should have received a copy of the GNU Lesser General Public License
 *	along with this program; if not, write to the Free Software
 *	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DIRECTIVES_H_
#define _DIRECTIVES_H_ 1

#include <libdodo/directives.runtime.h>

#ifdef __GNUC__
#define UNUSED __attribute__ (( unused ))
#else
#define UNUSED
#endif

/**
 * size of key for shared entities
 */
#ifdef __FreeBSD__
#define SH_KEY_SIZE             14
#else
#define SH_KEY_SIZE             32
#endif

/**
 * size of block for zLib de/compression
 */
#define ZLIB_CHUNK              100

/**
 * size of block for bzip2 de/compression
 */
#define BZIP_CHUNK              100

/**
 * file permissions for newly created file
 */
#define DEFAULT_FILE_PERM dodo::tools::filesystem::PERMISSION_OWNER_ALL_ACCESS

/**
 * size of string that represents numeric value during convertion
 */
#define NUMERIC_STRING_SIZE     20

/**
 * default socket timeouts for io operations
 */
#define IONETWORKCONNECTION_SOCKET_RECEIVE_TIMEOUT  1000
#define IONETWORKCONNECTION_SOCKET_SEND_TIMEOUT     1000

/**
 * default size of socket buffer for io operations
 */
#define IONETWORKCONNECTION_SOCKETBUFFER_INSIZE      4096
#define IONETWORKCONNECTION_SOCKETBUFFER_OUTSIZE     4096

/**
 * default linger options for network connections
 */
#define IONETWORKCONNECTION_SOCKET_LINGER_PERIOD     1
#define IONETWORKCONNECTION_SOCKET_LINGER_OPTION     LINGER_OPTION_WAIT_CLOSE

/**
 * default value of block size for io::channel operations
 */
#define IO_BLOCKSIZE            4096

/**
 * max line length to read from file in tools::filesystem
 */
#define LINE_MAXLEN             4096

/**
 * max length of the filesystem path
 */
#define PATH_MAXLEN              FILENAME_MAX

/**
 * max tries to print die message in tools:os::die
 */
#define DIE_MAXTRIES            5

/**
 * Limit of the size of the 'multipart/form-data' CGI POST request
 */
#define POST_SIZE_LIMIT         8388608

#ifdef NO_STRNLEN
#include <sys/types.h>

size_t strnlen(const char *s,
               size_t     maxlen);
#endif

/**
 * default thread stack size
 */
#define THREAD_STACK_SIZE 262144

#endif
