/***************************************************************************
 *            flushDisk.h
 *
 *  Tue Feb 22 08:19:57 2005
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
 
#ifndef _FLUSHDISK_H_
#define _FLUSHDISK_H_

#ifndef WIN
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <unistd.h>
	#include <dirent.h>
	#include <utime.h>
	#include <time.h>
#endif

#include "directives.h"
#include "flushDiskEx.h"
#include "types.h"

namespace dodo
{
	
	///modes of open file:
	enum flushDiskModesEnum
	{
		READ_ONLY,
		READ_WRITE,
		READ_WRITE_TRUNCATE
	};
	
	enum flushDiskFileTypeEnum
	{
		FILE_FILE,
		TMP_FILE,
		FIFO_FILE///use this type, if u want to create. if fifo is already created, no sense to create it again. if file is created it will not create it again; it is a check for file existanse, so if it's already created, it'll be slower!
	};
	
	enum permissionModesEnum
	{
		OWNER_READ_ACCESS = 2,
		GROUP_READ_ACCESS = 4,
		ALL_READ_ACCESS = 8,
		
		OWNER_WRITE_ACCESS = 16,
		GROUP_WRITE_ACCESS = 32,
		ALL_WRITE_ACCESS = 64,
		
		OWNER_EXECUTE_ACCESS = 128,
		GROUP_EXECUTE_ACCESS = 256,
		ALL_EXECUTE_ACCESS = 512,
				
		STICKY_ACCESS = 1024,
		
		SUID_ACCESS = 2048,
		SGID_ACCESS = 4096,
		
		OWNER_ALL_ACCESS = 14,
		GROUP_ALL_ACCESS = 112,
		ALL_ALL_ACCESS = 896,
		
		
	};
	
	/**
     * u may have two types of reading/writing 
	 * 	1)persistant - file opens when class constructs or where function setPersistant(true) was called. if parameter is false, file closes.
	 * 		if you want to work persistant with temp file, u should leave call constructor (bool,bool)
	 *	2)file opens every time u want ro read write
	 *	by default mode is READ_WRITE
	 *	reading/writing with string count{setNumberOfElements(long)} - number of characters(by default = 1)
	 *		if u once specified number of characters, u don't have to specify again
	 *	when u writing on the nonempty place by default it is error
	 *		u may specify that by 'over'
	 *	size - size of data to read/write in bytes(for string - length)
	 * 	for string size is one.
	 */
	
	class flushDisk
	{
		friend class flushDiskEx;///class of exception
		public:
			///constructors and destructors
			flushDisk(flushDiskFileTypeEnum type = FILE_FILE, const std::string &path = __string__, bool persisistant = true);
			flushDisk(bool persisistant);///if u want to work with temp file.
			virtual ~flushDisk();
			
			virtual bool open();///if opened previous file, closes it		
		
			virtual bool close();///closes file
			/**
			 * read functions. first argument - buffer, second - position
			 * returns false if nothing was read
			 */
			virtual bool read(std::string &data, unsigned long pos = 0);///reads to string; return false if eof
			virtual bool read(void *data, unsigned long pos = 0);///reads to void*; return false if eof		
			
			/**
			 * write functions
			 * first argument - buffer, second - position(if u want to add to end of the file set 'append' to true)
			 * returns false if exists, copy to buffer content of the node
			 */
			virtual bool write(const std::string &, unsigned long pos = 0);///writes string
			virtual bool write(const void * const , unsigned long pos = 0);///writes void*

			/**
			 * delete functions
			 * erase info on position
			 */
			virtual bool erase(unsigned long pos);///erase on position
			
			/**
			 * delete file/dir
			 * rename file/dir
			 * link file
			 * symbolic link
			 * chown
			 * chgrp
			 * touch
			 * mkdir
			 * change mode
			 * rmdir
			 */
			virtual bool unlink(const std::string &path);
			virtual bool rename(const std::string &oldPath, const std::string &newPath);
			virtual bool link(const std::string &oldPath, const std::string &newPath);
			virtual bool symlink(const std::string &oldPath, const std::string &newPath);
			virtual bool chown(const std::string &path, int uid);
			virtual bool chgrp(const std::string &path, int gid);
			virtual bool touch(const std::string &path, int time=-1);///now by default
			virtual bool mkdir(const std::string &path, int permissions);///see permissionModesEnum; use | to combine
			virtual bool chmod(const std::string &path, int permissions);///see permissionModesEnum; use | to combine
			virtual bool rmdir(const std::string &path);///recursive
			
			unsigned long size;///size of data
			bool persistant;///type of "connection"
			std::string path;///file name; for files only;
			bool over;///indicates whether overright; for files,tmp_files only
			flushDiskFileTypeEnum fileType;/// whether temp or not
			bool append;///if true, will append to the end of the file, even pos is set. false by default; for files only
			flushDiskModesEnum mode;///mode to open file
			
			std::string buffer;///before readin' or after writin' the storege sets to buffer if next option is set to true(bufferize); usefull for xExec
			bool bufferize;///false by default
			
		protected:
		
			mode_t getPermission(int permission);
			
			FILE *file;///file handler
			bool opened;///indicates whether file opens or not
	};
};

#endif
