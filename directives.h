/***************************************************************************
 *            directives.h
 *
 *  Fri Jul 15 22:19:48 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _DIRECTIVES_H_
#define _DIRECTIVES_H_

#define LIBDODO_VERSION "libdodo/0.0.3"

/**
 * !!!!note!!!!!
 * please read notes before uncommentin'!!!
 */


/** 
 * compile or use without exceptions. usage of exception depends of library compiled options; 
 * if u compiled library with exeptions u have to work with 'em;
 * if u compiled library without exeptions u have to work without 'em;
 */
#define NO_EX

/**
 * removes some checks
 * 1) check for request completeness(sqlBase)
 * 2) some checks that now is not needed, but in future releases will be neded, but set for now for more compability with newly writed code
 */
#define FAST

//#define UNICODE

//#define FLUSH_DISK_WO_XEXEC = faster, if u don't want to use it!
//#define MYSQLPP_WO_XEXEC

//#define WIN ///if u use window system


///socket default timeouts
#define RECIEVE_TIMEOUT 100
#define SEND_TIMEOUT 100

///default values of buffer for io operations(flush*)
#define INSIZE 1024
#define OUTSIZE 1024	

///default values of socket buffer for io operations(flush*)
#define SOCKET_INSIZE 1024
#define SOCKET_OUTSIZE 1024	


#endif /* _DIRECTIVES_H */
