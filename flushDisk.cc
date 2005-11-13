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
 
#include "flushDisk.h"

using namespace dodo;

flushDisk::flushDisk(flushDiskFileToCreateEnum type, 
					const std::string &a_path) : mode(READ_WRITE), 
									over(false), 
									fileType(type),
									path(a_path),
									append(false)
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
	_addPostExec(func, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushDisk::addPreExec(inExec func, 
					void *data) const
{
	_addPreExec(func, (dodoBase *)this, data);
}

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
	
	if (opened)
	{
		#ifndef FLUSH_DISK_WO_XEXEC
			performXExec(preExec);
		#endif
			
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
	#endif
	#ifndef WIN
		if (a_path.size()!=0 && strcmp(a_path.c_str(),path.c_str())!=0)
	#else
		if (a_path.size()!=0 && strcasecmp(a_path.c_str(),path.c_str())!=0)
	#endif
		path = a_path;
		
	if (opened)
		close();
	
	#ifndef FLUSH_DISK_WO_XEXEC	
		performXExec(preExec);	
	#endif
	
	///execute
	if (fileType == TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
			#else
				return false;
			#endif
		else
		{
			#ifndef WIN
				struct stat st;
			#else
				struct _stat st;
			#endif	
			
			bool exists(false);
			
			#ifndef WIN
				if (::lstat(path.c_str(),&st)==-1)
			#else
				if (::_stat(path.c_str(),&st)==-1)
			#endif
			{
				if (errno != ENOENT)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
					#else
						return false;							
					#endif
			}
			else
				exists = true;
				
			#ifndef WIN
				if (fileType == FIFO_FILE)
				{
					if (exists && !S_ISFIFO(st.st_mode))
						#ifndef NO_EX
							throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
						#else
							return false;
						#endif
					if (!exists)
						if (mkfifo(path.c_str(),flushDisk::getPermission(FILE_PERM)) == -1)
							#ifndef NO_EX
								throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
							#else
								return false;		
							#endif					
				}
				else
			#endif
				#ifndef WIN
					if ( (fileType == REG_FILE || fileType == TMP_FILE) && exists && !S_ISREG(st.st_mode) )
				#else
					if ( (fileType == REG_FILE || fileType == TMP_FILE) && exists && st.st_mode!=_S_IFREG )
				#endif
						#ifndef NO_EX
							throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__);
						#else
							return false;
			#endif
							
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
				case READ_ONLY:
				default:
					file = fopen(path.c_str(),"r");			
			}
		}
	}
	
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
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
flushDisk::read(void *a_void, 
				unsigned long a_pos) const
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_READ;
	#endif
	
	if (!opened)
		open();
	
	if (fileType == REG_FILE || fileType == TMP_FILE)
	{
		a_pos *= inSize;		
		if (fseek(file,a_pos,SEEK_SET) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	}
	
	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(preExec);
	#endif
	
	///execute 
	if (fileType == REG_FILE || fileType == TMP_FILE)
		fread(a_void,inSize,1,file);
	else
	#ifndef WIN
		#ifndef FAST
			if (fileType == FIFO_FILE)
		#endif
				fgets((char *)a_void,inSize,file);
	#else
		;			
	#endif
	
	#ifndef NO_EX
		switch (errno)
		{
			case EIO:
			case EINTR:
			case ENOMEM:
			case EOVERFLOW:
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		}
	#else	
		return false;
	#endif
	
	#ifndef FLUSH_DISK_WO_XEXEC		
		performXExec(postExec);
	#endif
	
	if (bufferize)
		buffer.assign((char *)a_void,inSize);
	
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
	
	this->read((void *)data,a_pos);
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
flushDisk::write(const void *const a_buf, 
				unsigned long a_pos)
{	
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_WRITE;
	#endif
	
	if (!opened)
		open();
	
	std::string stringToWrite((char *)a_buf);
		
	if (normalize)
		tools::normalize(stringToWrite,outSize);
			
	if (bufferize)
		buffer.assign(stringToWrite);
	
	if (fileType == REG_FILE || fileType == TMP_FILE)
	{
		register size_t read_bytes;
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
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
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
	
	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(preExec);
	#endif
	
	///execute 
	if (fileType == REG_FILE || fileType == TMP_FILE)
		fwrite(stringToWrite.c_str(),outSize,1,file);
	else
	#ifndef WIN
		#ifndef FAST
			if (fileType == FIFO_FILE)
		#endif
				fputs(stringToWrite.c_str(),file);
	#else
		;
	#endif
			
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
		return false;
	#endif
	
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
}
//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDisk::unlink(const std::string &path)
{
	register int status(0);
	#ifndef WIN
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) == -1)	
	#endif
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_UNLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	#ifndef WIN
		if (S_ISDIR(st.st_mode))
			status = ::rmdir(path.c_str());
		else
			status = ::unlink(path.c_str());
	#else
		if (st.st_mode != _S_IFDIR)
			status = ::_rmdir(path.c_str());
		else
			status = ::_unlink(path.c_str());		
	#endif	
	if (status == -1)
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
	if (::rename(oldPath.c_str(),newPath.c_str()) != -1)
		#ifdef NO_EX
			return true;
		#else
			return ;
		#endif
		
	#ifndef NO_EX
		throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RENAME,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
	#else
		return false;
	#endif	
	
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

#ifndef WIN

	#ifndef NO_EX
		void 
	#else
		bool
	#endif  
	flushDisk::link(const std::string &oldPath, 
					const std::string &newPath)
	{
		if  (::link(oldPath.c_str(),newPath.c_str()) != -1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif
			
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_LINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
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
						return true;
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
		if (::symlink(oldPath.c_str(),newPath.c_str()) != -1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif
			
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_SYMLINK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);		
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
	flushDisk::chown(const std::string &path, 
					int uid)
	{
		if (::chown(path.c_str(),uid,-1)!=-1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif
			
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CHOWN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);		
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
	flushDisk::chgrp(const std::string &path, 
					int gid)
	{
		if (::chown(path.c_str(),-1,gid) != -1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif
			
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CHGRP,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);		
		#else
			return false;
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
		{
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETUSEROWNER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return -1;		
			#endif
		}
		
		return st.st_uid;
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushDisk::getGroupOwner(const std::string &path)
	{
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
		{
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETGROUPOWNER,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
			#else
				return -1;				
			#endif
		}
		
		return st.st_gid;		
	}
#endif
						
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
	
	if (::utime(path.c_str(),&temp)!=-1)
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
	#ifndef WIN
		if (::mkdir(path.c_str(),getPermission(permissions)) == -1)
	#else
		if(::_mkdir(path.c_str(),getPermission(permissions)) == -1)
	#endif
	{
		if (force && (errno == EEXIST) )
		{
			#ifndef WIN
				struct stat st;
				if (::lstat(path.c_str(),&st) == -1)
			#else
				struct _stat st;
				if (::_stat(path.c_str(),&st) == -1)	
			#endif
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_MKDIR,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				#else
					return false;
				#endif
			
			#ifndef WIN
				if (S_ISDIR(st.st_mode))
					#ifdef NO_EX
						return true;
					#else
						return ;
					#endif
			#else
				if(st.st_mode!=_S_IFREG)		
					#ifdef NO_EX
						return true;
					#else
						return ;
					#endif
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
	#ifndef WIN
		if (::chmod(path.c_str(),getPermission(permissions)) == -1)
	#else
		if (::_chmod(path.c_str(),getPermission(permissions)) == -1)
	#endif
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
		#ifndef WIN
			mode |= S_IRUSR;	
		#else
			mode |= _S_IREAD;
		#endif
		
	#ifndef WIN
		if ((GROUP_READ_ACCESS & permission) == GROUP_READ_ACCESS)
			mode |= S_IRGRP;	
		if ((OTHER_READ_ACCESS & permission) == OTHER_READ_ACCESS)
			mode |= S_IROTH;
	#endif
			
	if ((OWNER_WRITE_ACCESS & permission) == OWNER_WRITE_ACCESS)
		#ifndef WIN
			mode |= S_IWUSR;
		#else
			mode |= _S_IWRITE;
		#endif
		
	#ifndef WIN	
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
	#endif	

	return mode;
		
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDisk::rm(const std::string &path)
{
	struct stat st;
	if (::lstat(path.c_str(),&st) == -1)
		if (errno != ENOENT)
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
		if (::unlink(path.c_str()) == -1)
		{
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif			
			
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif	
		}
	
	DIR *directory = opendir(path.c_str());
	
	if (directory == NULL)
	{
		#ifndef NO_EX
			if (errno == ENOENT)
					return ;
			else
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);		
		#else
			if (errno == ENOENT)
				return true;		
		#endif			
	}
	
	dirent *dd;
	std::string attached;
	
	while ( (dd=readdir(directory)) != NULL)
	{
		if ( (strcmp(dd->d_name,".") == 0) || (strcmp(dd->d_name,"..") == 0))
			continue;
		attached.assign(path+FILE_DELIM+dd->d_name);
		#ifndef WIN
			if (::lstat(attached.c_str(),&st) == -1)		
		#else
			if (::_stat(attached.c_str(),&st) == -1)		
		#endif
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif
		if (S_ISDIR(st.st_mode))
			flushDisk::rm(attached.c_str());
		else
			if (::unlink(attached.c_str()) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif
	}
	if (::rmdir(path.c_str()) == -1)
	#ifndef NO_EX
		throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_RM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
	#else
		return false;	
	#endif	
	
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

permissionModesEnum 
flushDisk::getPermissions(const std::string &path)
{
	#ifndef WIN
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) == -1)	
	#endif
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETPERMISSIONS,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return (permissionModesEnum)-1;
		#endif
		
	register int mode(NONE);
	
	#ifndef WIN
		if ((S_IRUSR & st.st_mode) == S_IRUSR)
	#else
		if ((_S_IREAD & st.st_mode) == _S_IREAD)
	#endif
		mode |= OWNER_READ_ACCESS;	
		
	#ifndef WIN
		if ((S_IRGRP & st.st_mode) == S_IRGRP)
			mode |= GROUP_READ_ACCESS;	
		if ((S_IROTH & st.st_mode) == S_IROTH)
			mode |= OTHER_READ_ACCESS;
	#endif	
		
	#ifndef WIN	
		if ((S_IWUSR & st.st_mode) == S_IWUSR)
	#else
		if ((_S_IWRITE & st.st_mode) == _S_IWRITE)		
	#endif
		mode |= OWNER_WRITE_ACCESS;
	
	#ifndef WIN
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
	#endif
		
	return (permissionModesEnum)mode;	
}

//-------------------------------------------------------------------

flushDiskFileTypeEnum 
flushDisk::getFileType(const std::string &path)
{
	#ifndef WIN
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) == -1)	
	#endif
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETFILETYPE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return (flushDiskFileTypeEnum)-1;
		#endif
	
	#ifndef WIN
		st.st_mode &= ~(S_IRWXU|S_IRWXG|S_IRWXO|S_ISUID|S_ISGID|S_ISVTX);
	#endif
	
	switch (st.st_mode)
	{
		#ifndef	WIN
			case S_IFREG:
		#else
			case _S_IFREG:
		#endif
			return REGULAR_FILE;
		#ifndef WIN
			case S_IFDIR:
		#else
			case _S_IFDIR:
		#endif
			return DIRECTORY;
		#ifndef WIN
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
		#endif
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
	#ifndef WIN
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) == -1)	
	#endif
	{
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_GETSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return -1;		
		#endif
		
		return -1;
	}
	return st.st_size;	
}

//-------------------------------------------------------------------

int 
flushDisk::getAccTime(const std::string &path)
{
	#ifndef WIN
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) == -1)	
	#endif
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
	#ifndef WIN
		struct stat st;
		if (::lstat(path.c_str(),&st) == -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) == -1)	
	#endif
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
	
	file.name.assign(basename((char *)path.c_str()));
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
