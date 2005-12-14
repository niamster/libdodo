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
#include <unistd.h>

#include <directives.h>

#include <flushDiskEx.h>
#include <types.h>
#include <tools.h>
#include <flush.h>

namespace dodo
{
	/**
	 * @enum flushDiskOperationTypeEnum describes type of operation for hook
	 */
	enum flushDiskOperationTypeEnum
	{
		FLUSHDISK_OPER_READ,
		FLUSHDISK_OPER_WRITE,
		FLUSHDISK_OPER_OPEN,
		FLUSHDISK_OPER_CLOSE
	};
	
	/**
	 * @enum flushDiskModesEnum descibes modes to open file
	 */
	enum flushDiskModesEnum
	{
		READ_ONLY,///< error if not exists file
		READ_WRITE,///< creates if not exists
		READ_WRITE_TRUNCATE,///< if exists = truncates
		APPEND///< for readin'; writin' to the end; you may skip parameter `pos` for write method
	};
	
	/**
	 * @enum flushDiskFileToCreateEnum describes file type you can create
	 */
	enum flushDiskFileToCreateEnum
	{
		REG_FILE,///< regular file
		TMP_FILE,///< temporary file. will be deleted after you exit program(or close it)
		FIFO_FILE///< FIFO file
	};
	
	/**
	 * @enum flushDiskFileTypeEnum indicates file type; you recieve it from getFileInfo and relative methods
	 */
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
	
	/**
	 * @enum permissionModesEnum describes permissions you can set/get to/from file
	 */
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
	 * @struct __fileInfo contains info about single file
	 */
	struct __fileInfo
	{
		std::string name;///< file name
		int perm;///< file permissions
		int type;///< file type
		long size;///< file size
		int modTime;///< modyfication time
		int accTime;///< access time
		int gid;///< group id of the file
		int uid;///< user id of the file
	};	
	
	/**
	 * @class flushDisk allows disk I/O manipulations
	 */
	
	class flushDisk : public flush
	{
		
		friend class flushSocket;
		
		private:
		
			/**
			 * cosrtructor prevents from copyin'
			 */
			flushDisk(flushDisk &fd);
		
		public:
							
			/**
			 * constructor
			 * @param type describes type of file with what manipulation will be made
			 * @param path is path to the file
			 * 
			 * if type == TMP_FILE, u don't have to specify path
			 */
			flushDisk(flushDiskFileToCreateEnum type, const std::string &path = __string__);
			
			/**
			 * destructor
			 */
			virtual ~flushDisk();
			
			#ifndef FLUSH_DISK_WO_XEXEC
			
				/**
				 * adds hook after the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */			
				virtual int addPostExec(inExec func, void *data) const;
				
				/**
				 * adds hook before the operation by callback
				 * @return number in list where function is set
				 * @param func is a pointer to function
				 * @param data is pointer to data toy want to pass to hook
				 */
				virtual int addPreExec(inExec func, void *data) const;
				
				#ifdef DL_EXT
	
					/**
					 * set function from module that will be executed before/after the main action call
					 * the type of hook[pre/post] is defined in module
					 * @return number in list where function is set
					 * @param func is a pointer to function
					 * @param data is pointer to data toy want to pass to hook
					 */			
					virtual int addExec(const std::string &module, void *data) const;
								
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPostExec(const std::string &module, void *data) const;
					
					/**
					 * adds hook after the operation by callback
					 * @return number in list where function is set
					 * @param module is a path to module, whrere hook exists
					 * @param data is pointer to data toy want to pass to hook
					 */
					virtual int addPreExec(const std::string &module, void *data) const;
					
				#endif	
			
			#endif
						
			/**
			 * opens file
			 * @param path describes path to file
			 * closes previous opened file if needed
			 * if u want to create pipe, but not a pipe was created with the same name - false will be returned
			 * if u want to create regular file, but not regular file was created with the same name - false will be returned
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
			 * read string
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							readString(std::string &data, unsigned long pos = 0) const;
			/**
			 * read data
			 * @param data will be filled with data
			 * @param pos indicates position in file
			 */							
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							read(char * const data, unsigned long pos = 0) const;///reads to void*; return false if eof		
			
			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */	
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							writeString(const std::string &data, unsigned long pos = 0);
										
			/**
			 * write string
			 * @param data will be written to file
			 * @param pos indicates position in file
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							write(const char * const data, unsigned long pos = 0);

			/**
			 * erase node on position
			 * @param pos indicates position in file
			 * @note for xexec  - no call for pre/postExec is performed, no operation type is set, 'cos it's only special type of write!!
			 */
			#ifndef NO_EX
				virtual void 
			#else
				virtual bool 
			#endif
							erase(unsigned long pos);
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
			 * copy file/empty dir/fifo...
			 * @param from specifies input file/empty dir/fifo...
			 * @param to specifies output file/empty dir/fifo...
			 * @param force indicates to overwrite
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							copy(const std::string &from, const std::string &to, bool force=false);
			
			/**
			 * copy file/Dir
			 * @param from specifies input file/Dir
			 * @param to specifies output file/Dir
			 * @param force indicates to overwrite
			 * @note if `to` exists - error
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							copyDir(const std::string &from, const std::string &to, bool force=false);
										
			/**
			 * @return basename of node
			 * @param is path to node
			 * basename => "/usr/lib" => "lib"
			 */
			static std::string basename(const std::string &path);
			
			/**
			 * @return dirname of node			 
			 * @param is path to node
			 * dirname => "/usr/lib" => "/usr"
			 */
			static std::string dirname(const std::string &path);
			
			/**
			 * @return file content in string
			 * @param path is path to file
			 */ 
			static std::string getFileContent(const std::string &path);

			/**
			 * @return file content in array of strings
			 * @param path is path to file
			 */ 			
			static stringArr getFileContentArr(const std::string &path);
			
			/**
			 * deletes file or notempty directory
			 * @param path is path to node
			 * @param force if it is true and nothing already exists do not say anything
			 */  
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							unlink(const std::string &path, bool force=true);///also empty directory
							
			/**
			 * rename file
			 * @param from specifies input file
			 * @param to specifies output file
			 */		
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							rename(const std::string &oldPath, const std::string &newPath);
			
			/**
			 * sets access and modyfiacation time of file
			 * @param path is path to file
			 * @param time is timestams; if not specified -> now
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							touch(const std::string &path, int time=-1);///now by default
			
			/**
			 * make directory
			 * @param path is path of directory to create
			 * @param permissions is new permissions; use | to combine; see permissionModesEnum
			 * @param force if it is true and directory already exists do not say anything
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							mkdir(const std::string &path, int permissions = OWNER_ALL_ACCESS, bool force = true);
			
			/**
			 * delete files, non empty directory
			 * @param path indicates the path to remove
			 * @param force if it is true and nothing already exists do not say anything
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							rm(const std::string &path, bool force=true);
			
			/**
			 * @return type of file; if error occured and lib was compiled without exceptions -> -1 will be returned;
			 * @param path is path to node
			 */				
			static flushDiskFileTypeEnum getFileType(const std::string &path);
			
			/**
			 * changes permissions
			 * @param path is path to node
			 * @param permissions is ne permissions; use | to combine; see permissionModesEnum
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							chmod(const std::string &path, int permissions);
			
			/**
			 * @return node permissions; if error occured and lib was compiled without exceptions -> -1 will be returned
			 * @param path is path to node
			 */				
			static permissionModesEnum getPermissions(const std::string &path);
			
			/**
			 * @return file size; if no such file or directory and lib was compiled without exceptions - will return -1
			 * @param path indicates path what to describe
			 */
			static long getSize(const std::string &path);///in bytes; 
			
			/**
			 * @return node access time
			 * @param path indicates path what to describe
			 */
			static int getAccTime(const std::string &path);			
			
			/**
			 * @return node modyfication time
			 * @param path indicates path what to describe
			 */
			static int getModTime(const std::string &path);
			
			/**
			 * gets original path of the file that link is set
			 * @param path indicates path what to describe
			 * @param original will be filled with path to file that link is set
			 */
			#ifndef NO_EX
				static void
			#else
				static bool
			#endif
								followSymlink(const std::string &path, std::string &original);
								
			/**
			 * creates symbolic link
			 * @param oldPath indicates original file path
			 * @param newPath indicates path to symlink
			 * @param force if is set to true link exists already - nothing to say, but replace
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							symlink(const std::string &oldPath, const std::string &newPath, bool force = true);

			/**
			 * creates link
			 * @param oldPath indicates original file path
			 * @param newPath indicates path to link
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							link(const std::string &oldPath, const std::string &newPath);
			
			/**
			 * change owner of the node
			 * @param path indacate the path of node where to change owner
			 * @param uid indicates user id
			 */				
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							chown(const std::string &path, int uid);
			
			/**
			 * change group owner of the node
			 * @param path indacate the path of node where to change group owner
			 * @param uid indicates group id
			 */									
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							chgrp(const std::string &path, int gid);
			
			/**
			 * @return user id of the node
			 * @param path indicates path where to get info
			 */
			static int getUserOwner(const std::string &path);

			/**
			 * @return group id of the node
			 * @param path indicates path where to get info
			 */			
			static int getGroupOwner(const std::string &path);
			
			/**
			 * @return file info; see __fileInfo
			 * @param path indicates path where to get info
			 */
			static __fileInfo getFileInfo(const std::string &path);///if no such file - empty will be returned

			/**
			 * @return file dir; see __fileInfo
			 * @param path indicates path where to get info
			 */
			static std::vector<__fileInfo> getDirInfo(const std::string &path);///if it'not a dir - empty will be returned and nothing write to 'dir' paramether!
			
			
			bool over;///< indicates whether overright or not; if tou want to write to nonempty node error will be occured; for files,tmp_files only
			mutable flushDiskModesEnum mode;///< mode to open file; if you change it then you have to reopen!
			
			/**
			 * @return path of the opened file
			 */			
			virtual std::string getPath() const;

			mutable flushDiskFileToCreateEnum fileType;///< type of file; if you change then it you have to reopen!
		
			/**
			 * @return system understandable permissions
			 * @param permission is user understandable permissions
			 */
			static int getPermission(int permission);
						
		private:
		
			mutable std::string path;///< file name
					
			mutable FILE *file;///< file handler
			
			mutable bool append;///< if true, will append to the end of the file, even pos is set.
	};

};

#endif
