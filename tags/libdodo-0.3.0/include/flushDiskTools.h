/***************************************************************************
 *            flushDiskTools.h
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
 
#ifndef _FLUSHDISKTOOLS_H_
#define _FLUSHDISKTOOLS_H_

#include <directives.h>
#include <types.h>

namespace dodo
{	
	/**
	 * @enum flushDiskFileTypeEnum indicates file type; you receive it from getFileInfo and relative methods
	 */
	enum flushDiskToolsFileTypeEnum
	{
		FILETYPE_REGULAR_FILE,
		FILETYPE_LOCAL_SOCKET,
		FILETYPE_SYMBOLIC_LINK,
		FILETYPE_BLOCK_DEVICE,
		FILETYPE_DIRECTORY,
		FILETYPE_CHARACTER_DEVICE,
		FILETYPE_FIFO
	};
	
	/**
	 * @enum permissionModesEnum describes permissions you can set/get to/from file
	 */
	enum permissionModesEnum
	{
		PERM_NONE = 0,
		
		PERM_OWNER_READ_ACCESS = 2,
		PERM_GROUP_READ_ACCESS = 4,
		PERM_OTHER_READ_ACCESS = 8,
		
		PERM_OWNER_WRITE_ACCESS = 16,
		PERM_GROUP_WRITE_ACCESS = 32,
		PERM_OTHER_WRITE_ACCESS = 64,
		
		PERM_OWNER_EXECUTE_ACCESS = 128,
		PERM_GROUP_EXECUTE_ACCESS = 256,
		PERM_OTHER_EXECUTE_ACCESS = 512,
				
		PERM_STICKY_ACCESS = 1024,
		
		PERM_SUID_ACCESS = 2048,
		PERM_SGID_ACCESS = 4096,
		
		PERM_OWNER_ALL_ACCESS = 146,
		PERM_GROUP_ALL_ACCESS = 292,
		PERM_OTHER_ALL_ACCESS = 584,
		
		PERM_ALL_ALL_ACCESS = 1022
		
	};
	
	/**
	 * @struct __fileInfo contains info about single file
	 */
	struct __fileInfo
	{
		std::string name;///< file name
		int perm;///< file permissions[see permissionModesEnum]; may be or'ed
		int type;///< file type[see flushDiskToolsFileTypeEnum]
		long size;///< file size
		long modTime;///< modyfication time
		long accTime;///< access time
		int gid;///< group id of the file
		int uid;///< user id of the file
	};	
	
	/**
	 * @class flushDisk allows disk I/O manipulations
	 */
	
	class flushDiskTools
	{
		
		friend class flushSocket;
		
		public:
			
			/**
			 * copy file/empty dir/fifo...
			 * @param from specifies input file/empty dir/fifo...
			 * @param to specifies output file/empty dir/fifo...
			 * @param force indicates to overwrite
			 * @note works as cp[but for files and empty dirs] -> if you copy to `bla/` it'll copy to dir `bla`, if to `bla` it'll overwrites
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
			 * @note works as cp -> if you copy to `bla/` it'll copy to dir `bla`, if to `bla` it'll overwrites
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							copyDir(const std::string &from, const std::string &to, bool force=false);
			
			/**
			 * appends string to file
			 * @param path describes file's path
			 * @param content describes content will be appended to file
			 */
			#ifndef NO_EX
				static void 
			#else
				static bool 
			#endif
							append(const std::string &path, const std::string &content);
										
			/**
			 * @return basename of node
			 * @param is path to node
			 * basename => "/usr/lib" => "lib"
			 */
			static std::string lastname(const std::string &path);
			
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
							mkdir(const std::string &path, int permissions = PERM_OWNER_ALL_ACCESS, bool force = true);
			
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
			static int getFileType(const std::string &path);
			
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
			static int getPermissions(const std::string &path);
			
			/**
			 * @return file size; if no such file or directory and lib was compiled without exceptions - will return -1
			 * @param path indicates path what to describe
			 */
			static long getSize(const std::string &path);///in bytes; 
			
			/**
			 * @return node access time
			 * @param path indicates path what to describe
			 */
			static long getAccTime(const std::string &path);			
			
			/**
			 * @return node modyfication time
			 * @param path indicates path what to describe
			 */
			static long getModTime(const std::string &path);
			
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
			 * @return true if file/directory exists
			 * @param path is path to file/directory
			 */				
			static bool exists(const std::string &path);
			
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
					
			/**
			 * @return system understandable permissions
			 * @param permission is user understandable permissions
			 */
			static int getPermission(int permission);
	};

};

#endif