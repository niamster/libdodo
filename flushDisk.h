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
	
	enum flushDiskOperationTypeEnum///for xExec
	{
		FLUSHDISK_OPER_READ,
		FLUSHDISK_OPER_WRITE,
		FLUSHDISK_OPER_OPEN,
		FLUSHDISK_OPER_CLOSE
	};
	
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
	 * info about single file
	 */
	struct __fileInfo
	{
		std::string name;
		int perm;
		int type;
		long size;
		int modTime;
		int accTime;
		
		#ifndef WIN	
			int gid;
			int uid;
		#endif
	};	
	
	/**
	 *	by default mode is READ_WRITE
	 *	when u writing on the nonempty place by default it is error
	 *		u may specify that by 'over'
	 *	size(In/Out) - size of data to (read/write) in bytes(for string - length)
	 */
	
	class flushDisk : public flush
	{
		
		friend class flushSocket;
		
		private:
		
			flushDisk(flushDisk &fd);///to prevent from copyin'
		
		public:
					
			/**
			 * return self, casted to base class - dodoBase; usefull to cast from child to parent;
			 */		
			virtual dodoBase * const getSelf();
							
			///constructors and destructors
			flushDisk(flushDiskFileToCreateEnum type, const std::string &path = __string__);///if type == TMP_FILE, u don't have to specify path
			virtual ~flushDisk();

			/**
			 * for xExec
			 */			
			virtual int addPostExec(inExec func, void *data) const;
			virtual int addPreExec(inExec func, void *data) const;
			
			/**
			 * closes previous opened file
			 * also creates file if it does not exists!
			 * if u want to create pipe, but regular file was created with the same name - false will be returned
			 * if u want to create regular file, but pipe was created with the same name - false will be returned
			 * if it's not temp file and PATH is not set - false will be returned
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif			 
							open(const std::string &path = __string__) const;///if opened previous file, closes it		
			
			/**
			 * closes file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							close() const;
							
			/**
			 * read functions. first argument - buffer, second - position
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readString(std::string &data, unsigned long pos = 0) const;///reads to string; return false if eof
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							read(char * const data, unsigned long pos = 0) const;///reads to void*; return false if eof		
			
			/**
			 * write functions
			 * first argument - buffer, second - position(if u want to add to end of the file set 'append' to true)
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeString(const std::string &data, unsigned long pos = 0);///writes string
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							write(const char * const data, unsigned long pos = 0);///writes void*

			/**
			 * erase info on position
			 * 
			 * NOTE for xexec  - no call for pre/postExec is performed, no operation type is set, 'cos it's only special type of write!!
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							erase(unsigned long pos);///erase on position
			/**
			 * flushes to disk
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							flush();
			/**
			 * delete file/dir (empty)
			 * rename file/dir
			 * touch (sets access/modify time)
			 * make dir
			 * remove file/dir (possible not empty)
			 * get file type
			 * change mode
			 * get permissions
			 * get size
			 * get access time
			 * get modyfication time
			 * resolve symlink
			 * symbolic link
			 * link file
			 * change owner
			 * change group
			 * get owner
			 * get group
			 * get file info
			 * get dir info
			 */
			  
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							unlink(const std::string &path);///also empty directory
			
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							rename(const std::string &oldPath, const std::string &newPath);
			
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							touch(const std::string &path, int time=-1);///now by default
			
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							mkdir(const std::string &path, int permissions = OWNER_ALL_ACCESS, bool force = true);///see permissionModesEnum; use | to combine; force - if created already - nothing to say
			
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							rm(const std::string &path);///recursive;
							
			static flushDiskFileTypeEnum getFileType(const std::string &path);///if error occured and lib was compiled without exceptions -> -1 will be returned; if unknown file/device on `path` - also -1 will be returned
			
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							chmod(const std::string &path, int permissions);///see permissionModesEnum; use | to combine
							
			static permissionModesEnum getPermissions(const std::string &path);///if error occured and lib was compiled without exceptions -> -1 will be returned
			
			static long getSize(const std::string &path);///in bytes; if no such file or directory - will return -1!

			static int getAccTime(const std::string &path);///timestamp
			static int getModTime(const std::string &path);///timestemp
			
			#ifndef NO_EX
				static void
			#else
				static bool
			#endif
								followSymlink(const std::string &path, std::string &original);///now not windows compatible
								
							
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							symlink(const std::string &oldPath, const std::string &newPath, bool force = true);///force - if created already - nothing to say, but replace(only if file to replaceis symlink too!!!)///now not windows compatible
			
			
			#ifndef WIN
			
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif
								link(const std::string &oldPath, const std::string &newPath);
								
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif
								chown(const std::string &path, int uid);
								
				#ifndef NO_EX
					static void 
				#else
					static bool 
				#endif
								chgrp(const std::string &path, int gid);
				
				static int getUserOwner(const std::string &path);
				
				static int getGroupOwner(const std::string &path);
				
			#endif
			
			static __fileInfo getFileInfo(const std::string &path);///if no such file - empty will be returned
			
			static std::vector<__fileInfo> getDirInfo(const std::string &path);///if it'not a dir - empty will be returned and nothing write to 'dir' paramether!;now is not WIN compatible
			
			
			bool over;///indicates whether overright; for files,tmp_files only
			bool append;///if true, will append to the end of the file, even pos is set. false by default; for files only
			mutable flushDiskModesEnum mode;///mode to open file
			
			/**
			 * returns path of the opened file
			 */			
			virtual std::string getPath() const;

			mutable flushDiskFileToCreateEnum fileType;/// see flushDiskFileToCreateEnum; if u change the ty u have to reopen!
			
		private:
		
			static int getPermission(int permission);
		
			mutable std::string path;///file name; for files only;
					
			mutable FILE *file;///file handler
	};

};

#endif
