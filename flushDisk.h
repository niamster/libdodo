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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>
#include <libgen.h>
	
#ifndef WIN
	#include <unistd.h>
#endif

#ifdef WIN
	#include <io.h>
#endif

#include "directives.h"
#include "flushDiskEx.h"
#include "types.h"
#include "tools.h"
#include "flush.h"

namespace dodo
{
	#define FILE_PERM (OWNER_READ_ACCESS|OWNER_WRITE_ACCESS)
	
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
		#ifndef WIN
			FIFO_FILE///use this type, if u want to create. if fifo is already created, no sense to create it again. if file is created it will not create it again; it is a check for file existanse, so if it's already created, it'll be slower!
		#endif
	};
	
	enum flushDiskFileTypeEnum
	{
		REGULAR_FILE,
		LOCAL_SOCKET,
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
		#ifndef WIN
			GROUP_READ_ACCESS = 4,
			OTHER_READ_ACCESS = 8,
		#endif
		
		OWNER_WRITE_ACCESS = 16,
		#ifndef WIN
			GROUP_WRITE_ACCESS = 32,
			OTHER_WRITE_ACCESS = 64,
		#endif
		
		#ifndef WIN
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
		#endif		
		
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
	
	struct __fileInfo
	{
		std::string name;
		int perm;
		int type;
		long size;
		int modTime;
		int accTime;
		int gid;
		int uid;
	};
	
	class flushDisk : protected flush
	{
		friend class flushDiskEx;///class of exception
		public:
			///constructors and destructors
			flushDisk(flushDiskFileToCreateEnum type, const std::string &path = __string__);///if type == TMP_FILE, u don't have to specify path
			virtual ~flushDisk();
			
			/**
			 * closes previous opened file
			 * also creates file if it does not exists!
			 * if u want to create pipe, but regular file was created with the same name - false will be returned
			 * if u want to create regular file, but pipe was created with the same name - false will be returned
			 * if it's not temp file and PATH is not set - false will be returned
			 */
			virtual bool open(const std::string &path = __string__) const;///if opened previous file, closes it		
		
			virtual bool close() const;///closes file
			/**
			 * read functions. first argument - buffer, second - position
			 * returns false if nothing was read
			 */
			virtual bool readString(std::string &data, unsigned long pos = 0) const;///reads to string; return false if eof
			virtual bool read(void *data, unsigned long pos = 0) const;///reads to void*; return false if eof		
			
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
			 * 
			 * NOTE for xexec  - no call for pre/postExec is performed, no operation type is set, 'cos it's only special type of write!!
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
			static bool touch(const std::string &path, int time=-1);///now by default
			static bool mkdir(const std::string &path, int permissions = OWNER_ALL_ACCESS, bool force = true);///see permissionModesEnum; use | to combine; force - if created already - nothing to say
			static bool rm(const std::string &path);///recursive; now is not WIN compatible
			static flushDiskFileTypeEnum getFileType(const std::string &path);///if error occured and lib was compiled without exceptions -> -1 will be returned; if unknown file/device on `path` - also -1 will be returned
			static bool chmod(const std::string &path, int permissions);///see permissionModesEnum; use | to combine
			static permissionModesEnum getPermissions(const std::string &path);///if error occured and lib was compiled without exceptions -> -1 will be returned
			
			static long getSize(const std::string &path);///in bytes; if no such file or directory - will return -1!

			static int getAccTime(const std::string &path);///timestamp
			static int getModTime(const std::string &path);///timestemp
			
			#ifndef WIN
				static bool link(const std::string &oldPath, const std::string &newPath);
				static bool symlink(const std::string &oldPath, const std::string &newPath, bool force = true);///force - if created already - nothing to say, but replace(only if file to replaceis symlink too!!!)
				static bool chown(const std::string &path, int uid);
				static bool chgrp(const std::string &path, int gid);
				
				static int getUserOwner(const std::string &path);
				static int getGroupOwner(const std::string &path);
			#endif
			
			static bool getFileInfo(const std::string &path, __fileInfo &file);///if no such file - false will be returned
			static bool getDirInfo(const std::string &path, std::vector<__fileInfo> &dir);///if it'not a dir - false will be returned and nothing write to 'dir' paramether!;now is not WIN compatible
			
			bool over;///indicates whether overright; for files,tmp_files only
			bool append;///if true, will append to the end of the file, even pos is set. false by default; for files only
			mutable flushDiskModesEnum mode;///mode to open file
						
			virtual std::string getPath() const;

			mutable flushDiskFileToCreateEnum fileType;/// see flushDiskFileToCreateEnum; if u change the ty u have to reopen!
			
			static int getPermission(int permission);
		private:
			mutable std::string path;///file name; for files only;
					
			mutable FILE *file;///file handler
	};

};

#endif
