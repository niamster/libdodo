/***************************************************************************
 *            flushDisk.cc
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
 
#include <flushDisk.h>

using namespace dodo;

flushDisk::flushDisk(flushDiskFileToCreateEnum type, 
					const std::string &a_path) : over(false),
												mode(READ_WRITE), 
												fileType(type),
												path(a_path), 
												append(false)
{
}

//-------------------------------------------------------------------

flushDisk::flushDisk(flushDisk &fd)
{
}

//-------------------------------------------------------------------

flushDisk::~flushDisk()
{
	close();
}

//-------------------------------------------------------------------

int 
flushDisk::addPostExec(inExec func, 
					void *data) const
{
	return _addPostExec(func, (void *)this, data);
}

//-------------------------------------------------------------------

int 
flushDisk::addPreExec(inExec func, 
					void *data) const
{
	return _addPreExec(func, (void *)this, data);
}

//-------------------------------------------------------------------

#ifdef DL_EXT

	int 
	flushDisk::addPostExec(const std::string &module, 
						void *data) const
	{
		return _addPostExec(module, (void *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushDisk::addPreExec(const std::string &module, 
						void *data) const
	{
		return _addPreExec(module, (void *)this, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushDisk::addExec(const std::string &module, 
						void *data) const
	{
		return _addExec(module, (void *)this, data);
	}

#endif

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushDisk::close() const 
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_CLOSE;
	#endif

	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(preExec);
	#endif
			
	if (opened)
	{			
		///execute
		if (fclose(file) != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif		
		
		#ifndef FLUSH_DISK_WO_XEXEC
			performXExec(postExec);
		#endif
					
		opened = false;
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
flushDisk::open(const std::string &a_path) const
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_OPEN;
		performXExec(preExec);	
	#endif

	if (a_path.size()!=0 && strcmp(a_path.c_str(),path.c_str())!=0)
		path = a_path;
		
	if (opened)
		close();
	
	///execute
	if (fileType == TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		else
		{
			struct stat st;
			bool exists(false);
			
			if (::lstat(path.c_str(),&st)==-1)
			{
				if (errno != ENOENT)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;							
					#endif
			}
			else
				exists = true;
			
			if (fileType == FIFO_FILE)
			{
				if (exists && !S_ISFIFO(st.st_mode))
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
				if (!exists)
					if (mkfifo(path.c_str(),flushDisk::getPermission(FILE_PERM)) == -1)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							return false;		
						#endif					
			}
			else
			{
				if ( (fileType == REG_FILE || fileType == TMP_FILE) && exists && !S_ISREG(st.st_mode))
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
					#else
						return false;
					#endif
			}
							
			switch (mode)
			{
				case READ_WRITE:
					file = fopen(path.c_str(),"r+");
					if (file == NULL)
						file = fopen(path.c_str(),"w+");
					break;
				case READ_WRITE_TRUNCATE:
					file = fopen(path.c_str(),"w+");
					break;
				case APPEND:
					file = fopen(path.c_str(),"a+");
					append = true;
					break;
				case READ_ONLY:
				default:
					file = fopen(path.c_str(),"r");			
			}
		}
	}
	
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	if (fileType == REG_FILE || fileType == TMP_FILE)
		flushDisk::chmod(path,FILE_PERM);
	
	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(postExec);
	#endif
		
	opened = true;
	
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
flushDisk::read(char * const a_void, 
				unsigned long a_pos) const
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_READ;
		performXExec(preExec);
	#endif
	
	if (fileType == REG_FILE || fileType == TMP_FILE)	
		if (fseek(file,a_pos*inSize,SEEK_SET) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	
	///execute 
	if (fileType == REG_FILE || fileType == TMP_FILE)
		fread(a_void,inSize,1,file);
	else
		#ifndef FAST
			if (fileType == FIFO_FILE)
		#endif
				fgets(a_void,inSize,file);
	
	#ifndef NO_EX
		switch (errno)
		{
			case EIO:
			case EINTR:
			case ENOMEM:
			case EOVERFLOW:
			case EROFS:
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
	
	buffer.assign(a_void,inSize);
	
	#ifndef FLUSH_DISK_WO_XEXEC		
		performXExec(postExec);
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
flushDisk::readString(std::string &a_str, 
				unsigned long a_pos) const
{
	register char *data = new char[inSize+1];
	if (data == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READSTRING,ERR_LIBDODO,FLUSHDISK_MEMORY_OVER,FLUSHDISK_MEMORY_OVER_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	memset(data,0,inSize);

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->read(data,a_pos);
	a_str.assign(data,inSize);
	
	delete [] data;
	
	#ifdef NO_EX	
		return result;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushDisk::writeString(const std::string &a_buf, 
				unsigned long a_pos)
{	
	return this->write(a_buf.c_str(),a_pos);	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDisk::write(const char *const a_buf, 
				unsigned long a_pos)
{
	register long oldOutSize = outSize;
	if (autoOutSize)
		outSize = strlen(a_buf);
		
	buffer.assign(a_buf, outSize);
				
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_WRITE;
		performXExec(preExec);
	#endif
		
	if (autoOutSize)
		outSize = buffer.size();
	
	if (fileType == REG_FILE || fileType == TMP_FILE)
	{
		register size_t read_bytes(-1);
		a_pos *= outSize;
		
		if (!over && !append)
		{		
			register char *t_buffer = new char[outSize*size_of_char];
			if (t_buffer == NULL)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_LIBDODO,FLUSHDISK_MEMORY_OVER,FLUSHDISK_MEMORY_OVER_STR,__LINE__,__FILE__);	
				#else
					return false;
				#endif
				
			if (fseek(file,a_pos,SEEK_SET) == -1)
				#ifndef NO_EX
				{
					delete [] t_buffer;
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				}
				#else
				{
					delete [] t_buffer;
					return false;
				}
				#endif
				
			read_bytes = fread(t_buffer,outSize,1,file);
			delete [] t_buffer;
		}
			
		if (!over && read_bytes != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_LIBDODO,FLUSHDISK_CANNOT_OVEWRITE,FLUSHDISK_CANNOT_OVEWRITE_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		if (append)
			if (fseek(file,0,SEEK_END) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
		else
			if (fseek(file,a_pos,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
	}
	
	///execute 
	if (fileType == REG_FILE || fileType == TMP_FILE)
		fwrite(buffer.c_str(),outSize,1,file);
	else
		#ifndef FAST
			if (fileType == FIFO_FILE)
		#endif
				fputs(buffer.c_str(),file);
			
	#ifndef NO_EX
		switch (errno)
		{
			case EIO:
			case EINTR:
			case ENOMEM:
			case EOVERFLOW:
			case EROFS:
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
	
	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(postExec);
	#endif
	
	outSize = oldOutSize;
			
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
flushDisk::erase(unsigned long a_pos)
{
	register char *empty = new char;
	if (empty == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_ERASE,ERR_LIBDODO,FLUSHDISK_MEMORY_OVER,FLUSHDISK_MEMORY_OVER_STR,__LINE__,__FILE__);	
		#else
			return false;
		#endif		

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->write(empty,a_pos);
	
	delete empty;
	
	#ifdef NO_EX
		return result;
	#endif
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushDisk::flush()
{
	if (fflush(file) != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_FLUSH,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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
flushDisk::unlink(const std::string &path,
				bool force)
{
	register int status(0);
	struct stat st;

	if (::lstat(path.c_str(),&st) == -1)
		if (errno != ENOENT || !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_UNLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_UNLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
flushDisk::rename(const std::string &oldPath, 
				const std::string &newPath)
{
	if (::rename(oldPath.c_str(),newPath.c_str()) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RENAME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
flushDisk::symlink(const std::string &oldPath, 
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
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_SYMLINK,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
				#endif
			else
				if (::unlink(newPath.c_str()) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_SYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
				#else
					return false;
				#endif				
	}

	if (::symlink(oldPath.c_str(),newPath.c_str()) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_SYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
flushDisk::link(const std::string &oldPath, 
				const std::string &newPath)
{
	if  (::link(oldPath.c_str(),newPath.c_str()) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_LINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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
flushDisk::chown(const std::string &path, 
				int uid)
{
	if (::chown(path.c_str(),uid,-1) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CHOWN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
flushDisk::chgrp(const std::string &path, 
				int gid)
{
	if (::chown(path.c_str(),-1,gid) == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CHGRP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#endif

	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

int 
flushDisk::getUserOwner(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETUSEROWNER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;		
		#endif
	
	return st.st_uid;
}

//-------------------------------------------------------------------

int 
flushDisk::getGroupOwner(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETGROUPOWNER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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
flushDisk::touch(const std::string &path,
				 int a_time)
{
	if (a_time==-1)
		a_time = time(NULL);
	
	utimbuf temp = {a_time, a_time};
	
	if (::utime(path.c_str(),&temp) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_TOUCH,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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
flushDisk::mkdir(const std::string &path, 
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
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_MKDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_MKDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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
flushDisk::chmod(const std::string &path, int permissions)
{
	if (::chmod(path.c_str(),getPermission(permissions)) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CHMOD,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif	
	
	#ifdef NO_EX
		return true;
	#endif		
}

//-------------------------------------------------------------------

int 
flushDisk::getPermission(int permission)
{
	register int mode(0);
	
	if ((OWNER_READ_ACCESS & permission) == OWNER_READ_ACCESS)
		mode |= S_IRUSR;
		
	if ((GROUP_READ_ACCESS & permission) == GROUP_READ_ACCESS)
		mode |= S_IRGRP;	
	if ((OTHER_READ_ACCESS & permission) == OTHER_READ_ACCESS)
		mode |= S_IROTH;
			
	if ((OWNER_WRITE_ACCESS & permission) == OWNER_WRITE_ACCESS)
		mode |= S_IWUSR;
		
	if ((GROUP_WRITE_ACCESS & permission) == GROUP_WRITE_ACCESS)
		mode |= S_IWGRP;
	if ((OTHER_WRITE_ACCESS & permission) == OTHER_WRITE_ACCESS)
		mode |= S_IWOTH;
		
	if ((STICKY_ACCESS & permission) == STICKY_ACCESS)
		mode |= S_ISVTX;
			
	if ((OWNER_EXECUTE_ACCESS & permission) == OWNER_EXECUTE_ACCESS)
		mode |= S_IXUSR;
	if ((GROUP_EXECUTE_ACCESS & permission) == GROUP_EXECUTE_ACCESS)
		mode |= S_IXGRP;
	if ((OTHER_EXECUTE_ACCESS & permission) == OTHER_EXECUTE_ACCESS)
		mode |= S_IXOTH;
		
	if ((SUID_ACCESS & permission) == SUID_ACCESS)
		mode |= S_ISUID;
	if ((SGID_ACCESS & permission) == SGID_ACCESS)
		mode |= S_ISGID;		

	return mode;
		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDisk::rm(const std::string &path,
				bool force)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)	
		if (errno != ENOENT || !force)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
							throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
						#else
							return false;		
						#endif
					
				if (S_ISDIR(st.st_mode))
					#ifndef NO_EX
						flushDisk::rm(attached.c_str());
					#else
						if (flushDisk::rm(attached.c_str()))
							return false;
					#endif
				else
					if (::unlink(attached.c_str()) == -1)
						if (errno != ENOENT || !force)
							#ifndef NO_EX
								throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
							#else
								return false;
							#endif
			}
			
			closedir(directory);
		
			if (::rmdir(path.c_str()) == -1)
				if (errno != ENOENT || !force)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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

permissionModesEnum 
flushDisk::getPermissions(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETPERMISSIONS,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return (permissionModesEnum)-1;
		#endif
		
	register int mode(NONE);
	
	if ((S_IRUSR & st.st_mode) == S_IRUSR)
		mode |= OWNER_READ_ACCESS;	
		
	if ((S_IRGRP & st.st_mode) == S_IRGRP)
		mode |= GROUP_READ_ACCESS;	
	if ((S_IROTH & st.st_mode) == S_IROTH)
		mode |= OTHER_READ_ACCESS;
	
	if ((S_IWUSR & st.st_mode) == S_IWUSR)		
		mode |= OWNER_WRITE_ACCESS;
	
	if ((S_IWGRP & st.st_mode) == S_IWGRP)
		mode |= GROUP_WRITE_ACCESS;
	if ((S_IWOTH & st.st_mode) == S_IWOTH)
		mode |= OTHER_WRITE_ACCESS;
			
	if ((S_ISVTX & st.st_mode) == S_ISVTX)
		mode |= STICKY_ACCESS;
			
	if ((S_IXUSR & st.st_mode) == S_IXUSR)
		mode |= OWNER_EXECUTE_ACCESS;
	if ((S_IXGRP & st.st_mode) == S_IXGRP)
		mode |= GROUP_EXECUTE_ACCESS;
	if ((S_IXOTH & st.st_mode) == S_IXOTH)
		mode |= OTHER_EXECUTE_ACCESS;
		
	if ((S_ISUID & st.st_mode) == S_ISUID)
		mode |= SUID_ACCESS;
	if ((S_ISGID & st.st_mode) == S_ISGID)
		mode |= SGID_ACCESS;		
		
	return (permissionModesEnum)mode;	
}

//-------------------------------------------------------------------

flushDiskFileTypeEnum 
flushDisk::getFileType(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILETYPE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return (flushDiskFileTypeEnum)-1;
		#endif
	
	st.st_mode &= ~(S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID|S_ISVTX);
	
	switch (st.st_mode)
	{
		case S_IFREG:
			return REGULAR_FILE;
			
		case S_IFDIR:
			return DIRECTORY;
			
		case S_IFLNK:
			return SYMBOLIC_LINK;
		case S_IFSOCK:
			return LOCAL_SOCKET;
		case S_IFBLK:
			return BLOCK_DEVICE;
		case S_IFCHR:
			return CHARACTER_DEVICE;
		case S_IFIFO:
			return FIFO;
			
		default:
			return (flushDiskFileTypeEnum)-1;
	}
}

//-------------------------------------------------------------------

std::string 
flushDisk::getPath() const
{
	return path;
}

//-------------------------------------------------------------------

long
flushDisk::getSize(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;		
		#endif

	return st.st_size;	
}

//-------------------------------------------------------------------

int 
flushDisk::getAccTime(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETACCTIME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;			
		#endif
		
	return (int)st.st_atim.tv_sec;		
}

//-------------------------------------------------------------------

int 
flushDisk::getModTime(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETMODTIME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;		
		#endif
		
	return (int)st.st_mtim.tv_sec;		
}

//-------------------------------------------------------------------

__fileInfo 
flushDisk::getFileInfo(const std::string &path)
{
	__fileInfo file;	
	
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILEINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return file;		
		#endif
		
	file.name.assign(::basename((char *)path.c_str()));
	file.type = flushDisk::getFileType(path);
	file.perm = flushDisk::getPermissions(path);
	file.accTime = flushDisk::getAccTime(path);
	file.modTime = flushDisk::getModTime(path);
	file.size = flushDisk::getSize(path);
	file.uid = flushDisk::getUserOwner(path);
	file.gid = flushDisk::getGroupOwner(path);
	
	return file;
}

//-------------------------------------------------------------------

std::vector<__fileInfo> 
flushDisk::getDirInfo(const std::string &path)
{
	std::vector<__fileInfo> dir;
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETDIRINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return dir;				
		#endif	
	
	if (!S_ISDIR(st.st_mode))
		return dir;
	
	
	DIR *directory = opendir(path.c_str());
	
	if (directory == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETDIRINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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
		dir.push_back(flushDisk::getFileInfo(attached));
	}
		
	return dir;	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void
#else
	bool
#endif
flushDisk::followSymlink(const std::string &path, 
						std::string &original)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_FOLLOWSYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif
	
	char buffer[MAXPATHLEN];
	
	if (!S_ISLNK(st.st_mode))			
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_SYMLINK,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
		#endif	
	
	if (::readlink(path.c_str(),buffer,MAXPATHLEN) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_FOLLOWSYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
	
	original.assign(buffer);
	
	#ifdef NO_EX
		return true;
	#endif
}
	
//-------------------------------------------------------------------

std::string 
flushDisk::getFileContent(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __string__;		
		#endif
	
	if (!S_ISREG(st.st_mode))
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENT,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
		#else
			return __string__;
		#endif	

	FILE *file = fopen(path.c_str(),"r");			
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISK_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISK_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return retS;
		#endif
	
	return retS;	
}

//-------------------------------------------------------------------

stringArr 
flushDisk::getFileContentArr(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENTARR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __stringarray__;		
		#endif
		
	if (!S_ISREG(st.st_mode))
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENTARR,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
		#else
			return __stringarray__;
		#endif

	FILE *file = fopen(path.c_str(),"r");
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENTARR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return __stringarray__;
		#endif
		
	register char buffer[DISK_MAXLINELEN];	
	stringArr arr;
	
	while (fgets(buffer,DISK_MAXLINELEN,file)!=NULL)
		arr.push_back(buffer);
		
	if (fclose(file) != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILECONTENTARR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return arr;
		#endif	
		
	return arr;
		
}

//-------------------------------------------------------------------

std::string 
flushDisk::basename(const std::string &path)
{
	return ::basename((char *)path.c_str());
}

//-------------------------------------------------------------------
			
std::string 
flushDisk::dirname(const std::string &path)
{
	return ::dirname((char *)path.c_str());
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool 
#endif
flushDisk::copy(const std::string &from, 
				const std::string &a_to, 
				bool force)
{
	std::string to = a_to;
	
	{	
		char *temp = ::basename((char *)to.c_str());
		if (strcmp(temp,"..")==0 || strcmp(temp,".")==0)
			to = temp + std::string(FILE_DELIM) + ::basename((char *)from.c_str());
	}	
	
	struct stat stFrom, stTo;
	
	if (::lstat(from.c_str(),&stFrom) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif

	if (::lstat(to.c_str(),&stTo) == -1)
	{
		if (errno != ENOENT)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;		
			#endif	
	}
	else
	{
		if (force)
		{
			#ifndef NO_EX
				flushDisk::rm(to);
			#else
				if (!flushDisk::rm(to))
					return false;
			#endif
		}
		else
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
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
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;		
				#endif			
		}
		else
			if (::mknod(to.c_str(),stFrom.st_mode,0)==-1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	
		FILE *toFile = fopen(to.c_str(),"w+");
		if (toFile == NULL)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
		
		register char buffer[INSIZE];
		
		register int i(0);	
		for (;i<iter;++i)
		{
			if (fseek(fromFile,i*INSIZE,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			if (fseek(toFile,i*INSIZE,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			if (fseek(fromFile,i*INSIZE,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			if (fseek(toFile,i*INSIZE,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
							throw baseEx(ERRMODULE_FLUSHSTD,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif	
			
		if (fclose(toFile) != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPY,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
flushDisk::copyDir(const std::string &from, 
					const std::string &a_to,
					bool force)
{
	std::string to = a_to;
	
	{	
		char *temp = ::basename((char *)to.c_str());
		if (strcmp(temp,"..")==0 || strcmp(temp,".")==0)
			to = temp + std::string(FILE_DELIM) + ::basename((char *)from.c_str());
	}	
	
	struct stat stFrom, stTo;
	
	if (::lstat(from.c_str(),&stFrom) == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPYDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif

	if (::lstat(to.c_str(),&stTo) == -1)
	{
		if (errno != ENOENT)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPYDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;		
			#endif
	}
	else
		if (force)
		{
			#ifndef NO_EX
				flushDisk::rm(to);
			#else
				if (!flushDisk::rm(to))
					return false;
			#endif
		}
		else
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPYDIR,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
			#else
				return false;		
			#endif
	
	if (!S_ISDIR(stFrom.st_mode))
	{
		#ifndef NO_EX
			flushDisk::copy(from,to,false);
		#else
			if (!flushDisk::copy(from,to,false))
				return false;
		#endif
	}
	else
	{
		#ifndef NO_EX
			flushDisk::mkdir(to,flushDisk::getPermissions(from));
		#else
			if(!flushDisk::mkdir(to,flushDisk::getPermissions(from)))
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
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_COPYDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);		
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
				flushDisk::copyDir(attachedFrom,attachedTo);
			#else
				if (!flushDisk::copyDir(attachedFrom,attachedTo))
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
