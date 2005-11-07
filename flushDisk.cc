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


flushDisk dummyDiskEx;///to throw ex in static members

//-------------------------------------------------------------------

dodoBase *
flushDisk::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

flushDisk::flushDisk()
{
}

//-------------------------------------------------------------------

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

#ifndef NO_EX
	void 
#else
	bool
#endif
flushDisk::close() const 
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSH_CLOSE;
	#endif
	
	if (opened)
	{
		#ifndef FLUSH_DISK_WO_XEXEC
			performXExec(preExec);
		#endif
			
		///execute
		register int ret = fclose(file);
		
		#ifndef NO_EX
			switch (ret)
			{
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, (flushDisk *)this,__LINE__,__FILE__);
				case EFBIG:
				case ENOSPC:
					throw flushDiskEx(FLUSHDISK_FILE_TOO_BIG, (flushDisk *)this,__LINE__,__FILE__);
				case EIO:
				case EPIPE:
				case ENXIO:
					throw flushDiskEx(FLUSHDISK_IOERROR, (flushDisk *)this,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN, (flushDisk *)this,__LINE__,__FILE__);
			}
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
		operType = FLUSH_CLOSE;
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
	
	#ifndef WIN
		umask(flushDisk::getPermission(FILE_PERM));	
	#else
		_umask(flushDisk::getPermission(FILE_PERM));	
	#endif		

	///execute
	if (fileType == TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			#ifndef NO_EX
				throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,(flushDisk *)this,__LINE__,__FILE__);
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
			#ifndef NO_EX
				switch (errno)
				{
					case EACCES:
						throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,(flushDisk *)this,__LINE__,__FILE__);
					case EIO:
						throw flushDiskEx(FLUSHDISK_IOERROR,(flushDisk *)this,__LINE__,__FILE__);
					case EINTR:
						throw flushDiskEx(FLUSHDISK_INTERRUPTED, (flushDisk *)this,__LINE__,__FILE__);
					case ENAMETOOLONG:
						throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,(flushDisk *)this,__LINE__,__FILE__);
					case ELOOP:
					case ENOTDIR:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,(flushDisk *)this,__LINE__,__FILE__);	
					case ENOMEM:
						throw flushDiskEx(FLUSHDISK_MEMORY_OVER,(flushDisk *)this,__LINE__,__FILE__);
					default:
						throw flushDiskEx(FLUSHDISK_UNKNOWN, (flushDisk *)this,__LINE__,__FILE__);
				}
			#else
				return false;							
			#endif
			else
				exists = true;
			#ifndef WIN
				if (fileType == FIFO_FILE)
				{
					if (exists && !S_ISFIFO(st.st_mode))
						#ifndef NO_EX
							throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,(flushDisk *)this,__LINE__,__FILE__);
						#else
							return false;
						#endif
					if (!exists)
						if (mkfifo(path.c_str(),flushDisk::getPermission(FILE_PERM)) == -1)
							#ifndef NO_EX
								switch (errno)
								{
									case EACCES:
										throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,(flushDisk *)this,__LINE__,__FILE__);
									case EIO:
										throw flushDiskEx(FLUSHDISK_IOERROR,(flushDisk *)this,__LINE__,__FILE__);
									case EINTR:
										throw flushDiskEx(FLUSHDISK_INTERRUPTED, (flushDisk *)this,__LINE__,__FILE__);
									case ENOSPC:
									case ENOMEM:
										throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,(flushDisk *)this,__LINE__,__FILE__);
									case EMFILE:
									case ENFILE:
										throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,(flushDisk *)this,__LINE__,__FILE__);
									case ENAMETOOLONG:
										throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,(flushDisk *)this,__LINE__,__FILE__);
									case ENOENT:
										throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,(flushDisk *)this,__LINE__,__FILE__);	
									case EROFS:
										throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,(flushDisk *)this,__LINE__,__FILE__);	
									default:
										throw flushDiskEx(FLUSHDISK_UNKNOWN, (flushDisk *)this,__LINE__,__FILE__);	
								}
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
							throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,(flushDisk *)this,__LINE__,__FILE__);
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
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,(flushDisk *)this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,(flushDisk *)this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, (flushDisk *)this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,(flushDisk *)this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,(flushDisk *)this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,(flushDisk *)this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,(flushDisk *)this,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,(flushDisk *)this,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN, (flushDisk *)this,__LINE__,__FILE__);		
		}
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
		operType = FLUSH_READ;
	#endif
	
	if (!opened)
		open();
	
	if (fileType == REG_FILE || fileType == TMP_FILE)
	{
		a_pos *= inSize;		
		fseek(file,a_pos,SEEK_SET);
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
				throw flushDiskEx(FLUSHDISK_IOERROR,(flushDisk *)this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, (flushDisk *)this,__LINE__,__FILE__);
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,(flushDisk *)this,__LINE__,__FILE__);
			case EOVERFLOW:
				throw flushDiskEx(FLUSHDISK_FILE_TOO_BIG,(flushDisk *)this,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN, (flushDisk *)this,__LINE__,__FILE__);			
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
		operType = FLUSH_WRITE;
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
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER, this,__LINE__,__FILE__);	
				#else
					return false;
				#endif
			fseek(file,a_pos,SEEK_SET);
			read_bytes = fread(t_buffer,outSize,1,file);
			delete [] t_buffer;
		}
			
		if (!over && read_bytes != 0)
			#ifndef NO_EX
				throw flushDiskEx(FLUSHDISK_CANNOT_OVEWRITE, this,__LINE__,__FILE__);	
			#else
				return false;
			#endif
		
		if (append)
			fseek(file,0,SEEK_END);
		else
			fseek(file,a_pos,SEEK_SET);
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
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EOVERFLOW:
				throw flushDiskEx(FLUSHDISK_FILE_TOO_BIG,this,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN, (flushDisk *)this,__LINE__,__FILE__);		
		}	
	#else
		return false;
	#endif
	
	if (fflush(file) != 0)
	#ifndef NO_EX
		switch (errno)
		{
			case EIO:
			case EAGAIN:
			case EPIPE:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EFBIG:
				throw flushDiskEx(FLUSHDISK_FILE_TOO_BIG,this,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);	
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN, (flushDisk *)this,__LINE__,__FILE__);	
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
			throw flushDiskEx(FLUSHDISK_MEMORY_OVER, this,__LINE__,__FILE__);	
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
			switch (errno)
			{							
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}	
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
		switch (errno)
		{
			case EACCES:
			case EPERM:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ENOENT:
			case EISDIR:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);	
			case ELOOP:
			case ENOTDIR:
			case EINVAL:
			case ENOTEMPTY:
				throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);		
		}
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
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);		
		}	
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ENOENT:
					throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENOTDIR:
				case EEXIST:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
			}	
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
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
					#endif
				else
					if (::unlink(newPath.c_str()) == -1)
					#ifndef NO_EX
						switch (errno)
						{
							case EACCES:
							case EPERM:
								throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
							case EIO:
								throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
							case EINTR:
								throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
							case ENOSPC:
							case ENOMEM:
								throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
							case EMFILE:
							case ENFILE:
								throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
							case ENAMETOOLONG:
								throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
							case ENOENT:
							case EISDIR:
								throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);	
							case ELOOP:
							case ENOTDIR:
							case EINVAL:
							case ENOTEMPTY:
								throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
							case EROFS:
								throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
							default:
								throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);			
						}
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ENOENT:
					throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENOTDIR:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
			}	
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ENOENT:
					throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);	
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);			
			}	
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ENOENT:
					throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
			}	
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
				switch (errno)
				{
					case EACCES:
						throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
					case EIO:
						throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
					case EINTR:
						throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
					case ENAMETOOLONG:
						throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
					case ELOOP:
					case ENOTDIR:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
					case ENOMEM:
						throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);	
				}
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
				switch (errno)
				{
					case EACCES:
						throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
					case EIO:
						throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
					case EINTR:
						throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
					case ENAMETOOLONG:
						throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
					case ELOOP:
					case ENOTDIR:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
					case ENOMEM:
						throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				}
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
		#ifdef NO_EX
			return true;
		#else
			return ;
		#endif
			
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
		}	
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
		if (::mkdir(path.c_str(),getPermission(permissions))!=-1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif
	#else
		if (::_mkdir(path.c_str(),getPermission(permissions))!=-1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif
	#endif
		
	if ( force && (errno == EEXIST) )
	{
		#ifndef WIN
			struct stat st;
			if (::lstat(path.c_str(),&st) == -1)
		#else
			struct _stat st;
			if (::_stat(path.c_str(),&st) == -1)	
		#endif
		#ifndef NO_EX
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
			}
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
		
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
			case EEXIST:
				throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
		}	
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
flushDisk::chmod(const std::string &path, int permissions)
{
	#ifndef WIN
		if (::chmod(path.c_str(),getPermission(permissions))!=-1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif
	#else
		if (::_chmod(path.c_str(),getPermission(permissions))!=-1)
			#ifdef NO_EX
				return true;
			#else
				return ;
			#endif	
	#endif
	
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
		}	
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
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ELOOP:
			case ENOTDIR:
				throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);	
		}
	#else
		return false;	
	#endif	
		
	if (!S_ISDIR(st.st_mode))
		if (::unlink(path.c_str()) == -1)
		{
			#ifndef NO_EX
				switch (errno)
				{
					case EACCES:
					case EPERM:
						throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
					case EIO:
						throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
					case EINTR:
						throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
					case ENOSPC:
					case ENOMEM:
						throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
					case EMFILE:
					case ENFILE:
						throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
					case ENAMETOOLONG:
						throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
					case EISDIR:
						throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);	
					case ELOOP:
					case ENOTDIR:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
					case EROFS:
						throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
					default:
						throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);			
				}
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
				case ENOENT:			
					#ifdef NO_EX
						return true;
					#else
						return ;
					#endif
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);		
			}	
		#else
			switch (errno)
			{
				case ENOENT:
					return true;
				default:
					return false;				
			}		
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}
		#else
			return false;		
		#endif
		if (S_ISDIR(st.st_mode))
			flushDisk::rm(attached.c_str());
		else
			if (::unlink(attached.c_str()) == -1)
			#ifndef NO_EX
				switch (errno)
				{
					case EACCES:
					case EPERM:
						throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
					case EIO:
						throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
					case EINTR:
						throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
					case ENOSPC:
					case ENOMEM:
						throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
					case EMFILE:
					case ENFILE:
						throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
					case ENAMETOOLONG:
						throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
					case EISDIR:
						throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);	
					case ELOOP:
					case ENOTDIR:
						throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);
					case EROFS:
						throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
					default:
						throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);		
				}
			#else
				return false;
			#endif
	}
	if (::rmdir(path.c_str()) == -1)
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);		
		}	
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}
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
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}
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
		if (::lstat(path.c_str(),&st) != -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) != -1)	
	#endif
	{
		#ifndef NO_EX
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}
		#else
			return -1;			
		#endif
		
		return -1;
	}
	return (int)st.st_atim.tv_sec;		
}

//-------------------------------------------------------------------

int 
flushDisk::getModTime(const std::string &path)
{
	#ifndef WIN
		struct stat st;
		if (::lstat(path.c_str(),&st) != -1)
	#else
		struct _stat st;
		if (::_stat(path.c_str(),&st) != -1)	
	#endif
	{
		#ifndef NO_EX
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ELOOP:
				case ENOTDIR:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}
		#else
			return -1;		
		#endif
		
		return -1;
	}
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
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ELOOP:
			case ENOTDIR:
				throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
			default:
				return file;				
		}
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
		switch (errno)
		{
			case ENOENT:
				return dir;
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
			case ELOOP:
			case ENOTDIR:
				throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyDiskEx,__LINE__,__FILE__);	
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_MEMORY_OVER,&dummyDiskEx,__LINE__,__FILE__);
			default:
				throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);;
		}
	#else
		return dir;				
	#endif	
	if (!S_ISDIR(st.st_mode))
		return dir;
	
	DIR *directory = opendir(path.c_str());
	
	if (directory == NULL)
	{
		#ifndef NO_EX
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyDiskEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyDiskEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyDiskEx,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyDiskEx,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyDiskEx,__LINE__,__FILE__);
				case ENOENT:
					throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyDiskEx,__LINE__,__FILE__);
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyDiskEx,__LINE__,__FILE__);
				default:
					throw flushDiskEx(FLUSHDISK_UNKNOWN,&dummyDiskEx,__LINE__,__FILE__);
			}	
		#else
			return dir;
		#endif			
	}
	
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
