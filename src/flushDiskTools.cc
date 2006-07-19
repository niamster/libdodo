/***************************************************************************
 *            flushDiskTools.cc
 *
 *  Wed Oct 8 01:44:18 2005
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
 
#include <flushDiskTools.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>

#include <tools.h>
#include <flushDiskToolsEx.h>

using namespace dodo;

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDiskTools::unlink(const std::string &path,
				bool force)
{
	register int status(0);
	struct stat st;

	if (::lstat(path.c_str(),&st) == -1)
		if (errno != ENOENT || !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_UNLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		

	if (S_ISDIR(st.st_mode))
		status = ::rmdir(path.c_str());
	else
		status = ::unlink(path.c_str());		

	if (status == -1)
		if (errno != ENOENT || !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_UNLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDiskTools::rename(const std::string &oldPath, 
				const std::string &newPath)
{
	if (::rename(oldPath.c_str(),newPath.c_str()) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_RENAME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#endif
	
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif  
flushDiskTools::symlink(const std::string &oldPath, 
					const std::string &newPath, 
					bool force)
{
	if (force)
	{
		struct stat st;
		if (::lstat(newPath.c_str(),&st) != -1)

			if (!S_ISLNK(st.st_mode))			
				#ifdef NO_EX
					return false;
				#else
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_SYMLINK,ERR_LIBDODO,FLUSHDISKTOOLS_WRONG_FILENAME,FLUSHDISKTOOLS_WRONG_FILENAME_STR,__LINE__,__FILE__);
				#endif
			else
				if (::unlink(newPath.c_str()) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_SYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
					#else
						return false;
					#endif				
	}

	if (::symlink(oldPath.c_str(),newPath.c_str()) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_SYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#endif	
	
	#ifdef NO_EX
		return true;
	#endif		
}					

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif  
flushDiskTools::link(const std::string &oldPath, 
				const std::string &newPath)
{
	if  (::link(oldPath.c_str(),newPath.c_str()) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_LINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
		#endif
	
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif  
flushDiskTools::chown(const std::string &path, 
				int uid)
{
	if (::chown(path.c_str(),uid,(unsigned int)-1) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_CHOWN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#endif
		
	#ifdef NO_EX
		return true;
	#endif			
}				

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif  
flushDiskTools::chgrp(const std::string &path, 
				int gid)
{
	if (::chown(path.c_str(),(unsigned int)-1,gid) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_CHGRP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#endif

	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

int 
flushDiskTools::getUserOwner(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETUSEROWNER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;		
		#endif
	
	return st.st_uid;
}

//-------------------------------------------------------------------

int 
flushDiskTools::getGroupOwner(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETGROUPOWNER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
		#else
			return -1;				
		#endif
	
	return st.st_gid;		
}
						
//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif  
flushDiskTools::touch(const std::string &path,
				 int a_time)
{
	if (a_time==-1)
		a_time = time(NULL);
	
	utimbuf temp = {a_time, a_time};
	
	if (::utime(path.c_str(),&temp) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_TOUCH,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
		#else
			return false;
		#endif
		
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif  
flushDiskTools::mkdir(const std::string &path, 
				int permissions,
				bool force)
{
	if (::mkdir(path.c_str(),getPermission(permissions)) == -1)
	{
		if (force && (errno == EEXIST) )
		{
			struct stat st;
			if (::lstat(path.c_str(),&st) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_MKDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
				
			if (S_ISDIR(st.st_mode))
				#ifdef NO_EX
					return true;
				#else
					return ;
				#endif
		}
		else
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_MKDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
			#else
				return false;
			#endif
	}
	
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif  
flushDiskTools::chmod(const std::string &path, int permissions)
{
	if (::chmod(path.c_str(),getPermission(permissions)) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_CHMOD,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
	
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

int 
flushDiskTools::getPermission(int permission)
{
	register int mode(0);
	
	if ((PERM_OWNER_READ_ACCESS & permission) == PERM_OWNER_READ_ACCESS)
		mode |= S_IRUSR;
		
	if ((PERM_GROUP_READ_ACCESS & permission) == PERM_GROUP_READ_ACCESS)
		mode |= S_IRGRP;	
	if ((PERM_OTHER_READ_ACCESS & permission) == PERM_OTHER_READ_ACCESS)
		mode |= S_IROTH;
			
	if ((PERM_OWNER_WRITE_ACCESS & permission) == PERM_OWNER_WRITE_ACCESS)
		mode |= S_IWUSR;
		
	if ((PERM_GROUP_WRITE_ACCESS & permission) == PERM_GROUP_WRITE_ACCESS)
		mode |= S_IWGRP;
	if ((PERM_OTHER_WRITE_ACCESS & permission) == PERM_OTHER_WRITE_ACCESS)
		mode |= S_IWOTH;
		
	if ((PERM_STICKY_ACCESS & permission) == PERM_STICKY_ACCESS)
		mode |= S_ISVTX;
			
	if ((PERM_OWNER_EXECUTE_ACCESS & permission) == PERM_OWNER_EXECUTE_ACCESS)
		mode |= S_IXUSR;
	if ((PERM_GROUP_EXECUTE_ACCESS & permission) == PERM_GROUP_EXECUTE_ACCESS)
		mode |= S_IXGRP;
	if ((PERM_OTHER_EXECUTE_ACCESS & permission) == PERM_OTHER_EXECUTE_ACCESS)
		mode |= S_IXOTH;
		
	if ((PERM_SUID_ACCESS & permission) == PERM_SUID_ACCESS)
		mode |= S_ISUID;
	if ((PERM_SGID_ACCESS & permission) == PERM_SGID_ACCESS)
		mode |= S_ISGID;		

	return mode;
		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDiskTools::rm(const std::string &path,
				bool force)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)	
		if (errno != ENOENT || !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;							
			#endif
		else
			#ifndef NO_EX
				return ;
			#else
				return false;							
			#endif

	if (!S_ISDIR(st.st_mode))
	{
		if (::unlink(path.c_str()) == -1)
			if (errno != ENOENT || !force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
	}
	else
	{			
		std::string attached;
		
		DIR *directory = opendir(path.c_str());
		
		if (directory == NULL)
		{
			if (errno != ENOENT || !force)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif	
		}
		else
		{
			dirent *dd;
			
			while ( (dd=readdir(directory)) != NULL)
			{
				if ( (strcmp(dd->d_name,".") == 0) || (strcmp(dd->d_name,"..") == 0))
					continue;
					
				attached.assign(path+FILE_DELIM+dd->d_name);
				
				if (::lstat(attached.c_str(),&st) == -1)
					if (errno != ENOENT || !force)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							return false;		
						#endif
					
				if (S_ISDIR(st.st_mode))
					#ifndef NO_EX
						flushDiskTools::rm(attached.c_str());
					#else
						if (flushDiskTools::rm(attached.c_str()))
							return false;
					#endif
				else
					if (::unlink(attached.c_str()) == -1)
						if (errno != ENOENT || !force)
							#ifndef NO_EX
								throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
							#else
								return false;
							#endif
			}
			
			closedir(directory);
		
			if (::rmdir(path.c_str()) == -1)
				if (errno != ENOENT || !force)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
					#else
						return false;	
					#endif	
		}
	}
	
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

int 
flushDiskTools::getPermissions(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETPERMISSIONS,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return (permissionModesEnum)-1;
		#endif
		
	register int mode(PERM_NONE);
	
	if ((S_IRUSR & st.st_mode) == S_IRUSR)
		mode |= PERM_OWNER_READ_ACCESS;	
		
	if ((S_IRGRP & st.st_mode) == S_IRGRP)
		mode |= PERM_GROUP_READ_ACCESS;	
	if ((S_IROTH & st.st_mode) == S_IROTH)
		mode |= PERM_OTHER_READ_ACCESS;
	
	if ((S_IWUSR & st.st_mode) == S_IWUSR)		
		mode |= PERM_OWNER_WRITE_ACCESS;
	
	if ((S_IWGRP & st.st_mode) == S_IWGRP)
		mode |= PERM_GROUP_WRITE_ACCESS;
	if ((S_IWOTH & st.st_mode) == S_IWOTH)
		mode |= PERM_OTHER_WRITE_ACCESS;
			
	if ((S_ISVTX & st.st_mode) == S_ISVTX)
		mode |= PERM_STICKY_ACCESS;
			
	if ((S_IXUSR & st.st_mode) == S_IXUSR)
		mode |= PERM_OWNER_EXECUTE_ACCESS;
	if ((S_IXGRP & st.st_mode) == S_IXGRP)
		mode |= PERM_GROUP_EXECUTE_ACCESS;
	if ((S_IXOTH & st.st_mode) == S_IXOTH)
		mode |= PERM_OTHER_EXECUTE_ACCESS;
		
	if ((S_ISUID & st.st_mode) == S_ISUID)
		mode |= PERM_SUID_ACCESS;
	if ((S_ISGID & st.st_mode) == S_ISGID)
		mode |= PERM_SGID_ACCESS;		
		
	return mode;	
}

//-------------------------------------------------------------------

int 
flushDiskTools::getFileType(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILETYPE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return (flushDiskToolsFileTypeEnum)-1;
		#endif
	
	st.st_mode &= ~(S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID|S_ISVTX);
	
	switch (st.st_mode)
	{
		case S_IFREG:
			return FILETYPE_REGULAR_FILE;
			
		case S_IFDIR:
			return FILETYPE_DIRECTORY;
			
		case S_IFLNK:
			return FILETYPE_SYMBOLIC_LINK;
		case S_IFSOCK:
			return FILETYPE_LOCAL_SOCKET;
		case S_IFBLK:
			return FILETYPE_BLOCK_DEVICE;
		case S_IFCHR:
			return FILETYPE_CHARACTER_DEVICE;
		case S_IFIFO:
			return FILETYPE_FIFO;
			
		default:
			return -1;
	}
}

//-------------------------------------------------------------------

long
flushDiskTools::getSize(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;		
		#endif

	return st.st_size;	
}

//-------------------------------------------------------------------

long
flushDiskTools::getAccTime(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETACCTIME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;			
		#endif
		
	return st.st_atime;		
}

//-------------------------------------------------------------------

long 
flushDiskTools::getModTime(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETMODTIME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;		
		#endif
		
	return st.st_mtime;		
}

//-------------------------------------------------------------------

__fileInfo 
flushDiskTools::getFileInfo(const std::string &path)
{
	__fileInfo file;	
	
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILEINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return file;		
		#endif
		
	file.name.assign(::basename((char *)path.c_str()));
	file.type = flushDiskTools::getFileType(path);
	file.perm = flushDiskTools::getPermissions(path);
	file.accTime = flushDiskTools::getAccTime(path);
	file.modTime = flushDiskTools::getModTime(path);
	file.size = flushDiskTools::getSize(path);
	file.uid = flushDiskTools::getUserOwner(path);
	file.gid = flushDiskTools::getGroupOwner(path);
	
	return file;
}

//-------------------------------------------------------------------

std::vector<__fileInfo> 
flushDiskTools::getDirInfo(const std::string &path)
{
	std::vector<__fileInfo> dir;
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETDIRINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return dir;				
		#endif	
	
	if (!S_ISDIR(st.st_mode))
		return dir;
	
	
	DIR *directory = opendir(path.c_str());
	
	if (directory == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETDIRINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
		#else
			return dir;
		#endif			
	
	dirent *dd;
	std::string attached;
	
	while ( (dd=readdir(directory)) != NULL)
	{
		if ( (strcmp(dd->d_name,".") == 0) || (strcmp(dd->d_name,"..") == 0))
			continue;
		attached.assign(path + FILE_DELIM + dd->d_name);
		dir.push_back(flushDiskTools::getFileInfo(attached));
	}
		
	return dir;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool
#endif
flushDiskTools::followSymlink(const std::string &path, 
						std::string &original)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_FOLLOWSYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif
	
	char buffer[MAXPATHLEN];
	
	if (!S_ISLNK(st.st_mode))			
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_SYMLINK,ERR_LIBDODO,FLUSHDISKTOOLS_WRONG_FILENAME,FLUSHDISKTOOLS_WRONG_FILENAME_STR,__LINE__,__FILE__);
		#endif	
	
	int count = 0;
	
	if ( (count = ::readlink(path.c_str(),buffer,MAXPATHLEN)) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_FOLLOWSYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
	
	original.assign(buffer,count);
	
	#ifdef NO_EX
		return true;
	#endif
}
	
//-------------------------------------------------------------------

std::string 
flushDiskTools::getFileContent(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __string__;		
		#endif
	
	if (!S_ISREG(st.st_mode))
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENT,ERR_LIBDODO,FLUSHDISKTOOLS_WRONG_FILENAME,FLUSHDISKTOOLS_WRONG_FILENAME_STR,__LINE__,__FILE__);
		#else
			return __string__;
		#endif	

	FILE *file = fopen(path.c_str(),"r");			
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __string__;
		#endif
	
	register char buffer[INSIZE];
	
	register long iter = st.st_size/INSIZE, rest = st.st_size%INSIZE;
	std::string retS = "";	
	
	register int i(0);	
	for (;i<iter;++i)
	{
		if (fseek(file,i*INSIZE,SEEK_SET) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return retS;
			#endif
			
		if (fread(buffer,INSIZE,1,file)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISKTOOLS_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				}	
			#else			
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:	
					case EROFS:
						return retS;
				}
			#endif

		retS.append(buffer,INSIZE);
	}
	if (rest>0)
	{
		if (fseek(file,i*INSIZE,SEEK_SET) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return retS;
			#endif
					
		if (fread(buffer,rest,1,file)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISKTOOLS_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				}	
			#else			
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:	
					case EROFS:
						return retS;
				}
			#endif
			
		retS.append(buffer,rest);	
	}

	if (fclose(file) != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return retS;
		#endif
	
	return retS;	
}

//-------------------------------------------------------------------

stringArr 
flushDiskTools::getFileContentArr(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENTARR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __stringarray__;		
		#endif
		
	if (!S_ISREG(st.st_mode))
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENTARR,ERR_LIBDODO,FLUSHDISKTOOLS_WRONG_FILENAME,FLUSHDISKTOOLS_WRONG_FILENAME_STR,__LINE__,__FILE__);
		#else
			return __stringarray__;
		#endif

	FILE *file = fopen(path.c_str(),"r");
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENTARR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __stringarray__;
		#endif
		
	register char buffer[DISK_MAXLINELEN];	
	stringArr arr;
	
	while (fgets(buffer,DISK_MAXLINELEN,file)!=NULL)
		arr.push_back(buffer);
		
	if (fclose(file) != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_GETFILECONTENTARR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return arr;
		#endif	
		
	return arr;
		
}

//-------------------------------------------------------------------

std::string 
flushDiskTools::lastname(const std::string &path)
{
	char tempB[MAXPATHLEN];
		
	strcpy(tempB,path.c_str());
	
	::basename((char *)path.c_str());
	
	std::string result(tempB);
	
	return result;
}

//-------------------------------------------------------------------
			
std::string 
flushDiskTools::dirname(const std::string &path)
{
	char tempB[MAXPATHLEN];	
	
	strcpy(tempB,path.c_str());
	
	::dirname((char *)path.c_str());
	
	std::string result(tempB);
	
	return result;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif
flushDiskTools::copy(const std::string &from, 
				const std::string &a_to, 
				bool force)
{
	std::string to = a_to;
	
	{
		char tempB[MAXPATHLEN];	
			
		strcpy(tempB,to.c_str());
		char *toT = ::basename(tempB);
		if (strcmp(toT,"..")==0 || strcmp(toT,".")==0 || a_to[a_to.length()-1] == FILE_DELIM)
			to = toT + std::string(1,FILE_DELIM) + ::basename((char *)from.c_str());
	}
	
	struct stat stFrom, stTo;
	
	if (::lstat(from.c_str(),&stFrom) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif
		
	if (::lstat(to.c_str(),&stTo) == -1)
	{
		if (errno != ENOENT)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;		
			#endif	
	}
	else
	{
		if (force)
		{
			if (!S_ISDIR(stTo.st_mode))
			{
				if (::unlink(to.c_str()) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_UNLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
			else
				if (::rmdir(to.c_str()) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_UNLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;
					#endif
			
		}
		else
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_LIBDODO,FLUSHDISKTOOLS_WRONG_FILENAME,FLUSHDISKTOOLS_WRONG_FILENAME_STR,__LINE__,__FILE__);
			#else
				return false;		
			#endif
	}
	
	if (!S_ISREG(stFrom.st_mode))
	{
		if (S_ISDIR(stFrom.st_mode))
		{
			if (::mkdir(to.c_str(),stFrom.st_mode)==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;		
				#endif			
		}
		else
			if (S_ISLNK(stFrom.st_mode))
			{
				register char buffer[MAXPATHLEN];
				register int count = 0;
	
				if ( (count = ::readlink(from.c_str(),buffer,MAXPATHLEN)) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;		
					#endif
				
				buffer[count] = '\0';
					
				if (::symlink(buffer,to.c_str()) == -1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;		
					#endif					
			}
			else
				if (::mknod(to.c_str(),stFrom.st_mode,0)==-1)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;		
					#endif	
	}
	else
	{
		register long iter = stFrom.st_size/INSIZE, rest = stFrom.st_size%INSIZE;
		
		FILE *fromFile = fopen(from.c_str(),"r");			
		if (fromFile == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
			
		FILE *toFile = fopen(to.c_str(),"w+");
		if (toFile == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		register char buffer[INSIZE];
		
		register int i(0),j;
		for (;i<iter;++i)
		{
			j = i*INSIZE;
			
			if (fseek(fromFile,j,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			if (fseek(toFile,j,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
							
			if (fread(buffer,INSIZE,1,fromFile)==0)
				#ifndef NO_EX
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:
						case EROFS:
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}	
				#else			
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:	
						case EROFS:
							return false;
					}
				#endif
	
			if (fwrite(buffer,INSIZE,1,toFile)==0)
				#ifndef NO_EX
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:
						case EROFS:
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}	
				#else			
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:	
						case EROFS:
							return false;
					}
				#endif
		}
		if (rest>0)
		{
			j = i*INSIZE;
			if (fseek(fromFile,j,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			if (fseek(toFile,j,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
									
			if (fread(buffer,rest,1,fromFile)==0)
				#ifndef NO_EX
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:
						case EROFS:
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}	
				#else			
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:	
						case EROFS:
							return false;
					}
				#endif
	
			if (fwrite(buffer,rest,1,toFile)==0)
				#ifndef NO_EX
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:
						case EROFS:
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					}	
				#else			
					switch (errno)
					{
						case EIO:
						case EINTR:
						case ENOMEM:
						case EOVERFLOW:	
						case EROFS:
							return false;
					}
				#endif					
		}
			
		if (fclose(fromFile) != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif	
			
		if (fclose(toFile) != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	}
				
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif
flushDiskTools::copyDir(const std::string &from, 
					const std::string &a_to,
					bool force)
{
	std::string to = a_to;
	
	{
		char tempB[MAXPATHLEN];	
			
		strcpy(tempB,to.c_str());
		char *toT = ::basename(tempB);
		if (strcmp(toT,"..")==0 || strcmp(toT,".")==0 || a_to[a_to.length()-1] == FILE_DELIM)
			to = toT + std::string(1,FILE_DELIM) + ::basename((char *)from.c_str());
	}
	
	struct stat stFrom, stTo;
	
	if (::lstat(from.c_str(),&stFrom) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPYDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif

	if (::lstat(to.c_str(),&stTo) == -1)
	{
		if (errno != ENOENT)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPYDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	}
	else
		if (force)
		{
			#ifndef NO_EX
				flushDiskTools::rm(to,force);
			#else
				if (!flushDiskTools::rm(to,force))
					return false;
			#endif
		}
		else
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPYDIR,ERR_LIBDODO,FLUSHDISKTOOLS_WRONG_FILENAME,FLUSHDISKTOOLS_WRONG_FILENAME_STR,__LINE__,__FILE__);
	        #else
				return false;		
			#endif
	
	if (!S_ISDIR(stFrom.st_mode))
	{
		#ifndef NO_EX
			flushDiskTools::copy(from,to,force);
		#else
			if (!flushDiskTools::copy(from,to,force))
				return false;
		#endif
	}
	else
	{
		if (::mkdir(to.c_str(),stFrom.st_mode) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPYDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
			#else
				return false;		
			#endif
	
		std::string attachedFrom,attachedTo;
		
		DIR *directory = opendir(from.c_str());
		
		if (directory == NULL)
		{
			#ifndef NO_EX
				if (errno == ENOENT)
						return ;
				else
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_COPYDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);		
			#else
				if (errno == ENOENT)
					return true;
				else
					return false;		
			#endif			
		}
		
		dirent *dd;
		
		while ( (dd=readdir(directory)) != NULL)
		{
			if ( (strcmp(dd->d_name,".") == 0) || (strcmp(dd->d_name,"..") == 0))
				continue;
				
			attachedTo.assign(to+FILE_DELIM+dd->d_name);
			attachedFrom.assign(from+FILE_DELIM+dd->d_name);

			#ifndef NO_EX
				flushDiskTools::copyDir(attachedFrom,attachedTo,force);
			#else
				if (!flushDiskTools::copyDir(attachedFrom,attachedTo,force))
					return false;
			#endif
		}
		
		closedir(directory);
	}
					
	#ifdef NO_EX
		return true;
	#endif
}

//-------------------------------------------------------------------

bool 
flushDiskTools::exists(const std::string &path)
{
	struct stat st;
	
	if (::lstat(path.c_str(),&st) == 0)
		return true;
	
	return false;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif
flushDiskTools::append(const std::string &path, 
						const std::string &content)
{
	FILE *file = fopen(path.c_str(),"a+");			
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_APPEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	if (fwrite(content.c_str(), content.size(), 1, file) == 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS,FLUSHDISKTOOLS_APPEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------
