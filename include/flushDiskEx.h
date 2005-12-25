/***************************************************************************
 *            flushDiskEx.h
 *
 *  Mon Feb 21 03:03:47 2005
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _FLUSHDISKEX_H_
#define _FLUSHDISKEX_H_

#include <directives.h>

#include <baseEx.h>

#ifndef NO_EX

	namespace dodo
	{	
		
		/**
		 * libdodo defined errors
		 */
		enum flushDiskExR
		{
			FLUSHDISK_WRONG_FILENAME,
			FLUSHDISK_CANNOT_OVEWRITE,
		};
		
		/**
		 * libdodo defined errors' explanation
		 */
		#define FLUSHDISK_WRONG_FILENAME_STR "Probably wrong filename, type of file is wrong or `path` is empty!"
		#define FLUSHDISK_CANNOT_OVEWRITE_STR "You set option not to overwrite, but try to do that =)"
		
		/**
		 * ID of function where exception was thrown
		 */
		enum flushDiskFunctionsID
		{
			FLUSHDISK_CLOSE,
			FLUSHDISK_OPEN,
			FLUSHDISK_READ,
			FLUSHDISK_WRITE,
			FLUSHDISK_FLUSH,
		};
	};

#endif	
	
#endif
