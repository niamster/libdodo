/***************************************************************************
 *            flushDisk.h
 *
 *  Tue Oct 8 08:19:57 2005
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
#include "flush.h"

namespace dodo
{
	
	///modes of open file:
	enum flushDiskModesEnum
	{
		READ_ONLY,
		READ_WRITE,
		READ_WRITE_TRUNCATE
	};
	
	enum flushDiskFileToCreateEnum
	{
		REG_FILE,
		TMP_FILE,
		FIFO_FILE///use this type, if u want to create. if fifo is already created, no sense to create it again. if file is created it will not create it again; it is a check for file existanse, so if it's already created, it'll be slower!
	};
	
	enum flushDiskFileTypeEnum
	{
		REGULAR_FILE,
		UNIX_SOCKET,
		SYMBOLIC_LINK,
		BLOCK_DEVICE,
		DIRECTORY,
		CHARACTER_DEVICE,
		FIFO
	};
	
	enum permissionModesEnum
	{
		NONE = 0,
		
		OWNER_READ_ACCESS = 2,
		GROUP_READ_ACCESS = 4,
		OTHER_READ_ACCESS = 8,
		
		OWNER_WRITE_ACCESS = 16,
		GROUP_WRITE_ACCESS = 32,
		OTHER_WRITE_ACCESS = 64,
		
		OWNER_EXECUTE_ACCESS = 128,
		GROUP_EXECUTE_ACCESS = 256,
		OTHER_EXECUTE_ACCESS = 512,
				
		STICKY_ACCESS = 1024,
		
		SUID_ACCESS = 2048,
		SGID_ACCESS = 4096,
		
		OWNER_ALL_ACCESS = 146,
		GROUP_ALL_ACCESS = 292,
		OTHER_ALL_ACCESS = 584,
		
		ALL_ALL_ACCESS = 1022
		
		
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
	
	class flushDisk : public flush
	{
		friend class flushDiskEx;///class of exception
		public:
			///constructors and destructors
			flushDisk(flushDiskFileToCreateEnum type = REG_FILE, const std::string &path = __string__, bool persisistant = true);
			flushDisk(bool persisistant);///if u want to work with temp file.
			virtual ~flushDisk();
			
			virtual bool open();///if opened previous file, closes it		
		
			virtual bool close();///closes file
			/**
			 * read functions. first argument - buffer, second - position
			 * returns false if nothing was read
			 */
			virtual bool readString(std::string &data, unsigned long pos = 0);///reads to string; return false if eof
			virtual bool read(void *data, unsigned long pos = 0);///reads to void*; return false if eof		
			
			/**
			 * write functions
			 * first argument - buffer, second - position(if u want to add to end of the file set 'append' to true)
			 * returns false if exists, copy to buffer content of the node
			 */
			virtual bool writeString(const std::string &, unsigned long pos = 0);///writes string
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
			/**
			 * in next functions(if u compiled library with exceptions) i used empty flushDisk class copy to make them static and to use defined exception class
			 */
			static bool unlink(const std::string &path);///also empty directory
			static bool rename(const std::string &oldPath, const std::string &newPath);
			static bool link(const std::string &oldPath, const std::string &newPath);
			static bool symlink(const std::string &oldPath, const std::string &newPath, bool force = true);///force - if created already - nothing to say, but replace(only if file to replaceis symlink too!!!)
			static bool chown(const std::string &path, int uid);
			static bool chgrp(const std::string &path, int gid);
			static bool touch(const std::string &path, int time=-1);///now by default
			static bool mkdir(const std::string &path, int permissions = OWNER_ALL_ACCESS, bool force = true);///see permissionModesEnum; use | to combine; force - if created already - nothing to say
			static bool chmod(const std::string &path, int permissions);///see permissionModesEnum; use | to combine
			static bool rmdir(const std::string &path);///recursive
			static permissionModesEnum getPermissions(const std::string &path);///if error occured and lib was compiled without exceptions -> -1 will be returned
			static flushDiskFileTypeEnum getFileType(const std::string &path);///if error occured and lib was compiled without exceptions -> -1 will be returned; if unknown file/device on `path` - also -1 will be returned
			
			bool persistant;///type of "connection"
			std::string path;///file name; for files only;
			bool over;///indicates whether overright; for files,tmp_files only
			flushDiskFileToCreateEnum fileType;/// whether temp or not
			bool append;///if true, will append to the end of the file, even pos is set. false by default; for files only
			flushDiskModesEnum mode;///mode to open file
						
		protected:
		
			static mode_t getPermission(int permission);
			
			FILE *file;///file handler
	};
};

#endif
