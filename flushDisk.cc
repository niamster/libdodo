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

flushDisk dummyEx(TMP_FILE);

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

bool
flushDisk::close() const 
{
	if (opened)
	{
		register int ret = fclose(file);
		#ifndef NO_EX
			switch (ret)
			{
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, (flushDisk *)this,__LINE__,__FILE__);
			}
		#else
			switch (ret)
			{
				case EFBIG:
				case EINTR:
				case ENOSPC:
					return false;
			}			
		#endif			
		opened = false;
	}
}

//-------------------------------------------------------------------

bool
flushDisk::open(const std::string &a_path) const
{
	if (a_path.size()!=0 && strcmp(a_path.c_str(),path.c_str())!=0)
		path = a_path;
	if (opened)
		close();
	if (fileType == TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			return false;
		else
		{
			struct stat st;
			bool exists(false);
			if (stat(path.c_str(),&st)==-1)
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
				}
			#else
				switch (errno)
				{
					case EACCES:
					case EIO:
					case EINTR:
					case ENAMETOOLONG:
						return false;					
				}							
			#endif
			else
				exists = true;
			
			if (fileType == FIFO_FILE)
			{
				if (exists && !S_ISFIFO(st.st_mode))
					return false;
				umask(0);
				if (!exists)
					if (mkfifo(path.c_str(),0666) == -1)
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
						}
					#else
						switch (errno)
						{
							case EROFS:
							case EACCES:
							case EIO:
							case EINTR:
							case ENOSPC:
							case ENOMEM:
							case EMFILE:
							case ENFILE:
							case ENAMETOOLONG:
							case ENOENT:
								return false;					
						}
					#endif					
			}
			else
				if ( (fileType == REG_FILE || fileType == TMP_FILE) && exists && !S_ISREG(st.st_mode))
					return false;
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
		}
	#else
		switch (errno)
		{
			case EROFS:
			case EACCES:
			case EIO:
			case EINTR:
			case ENOSPC:
			case ENOMEM:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case ENOENT:
				return false;					
		}			
	#endif
	opened = true;
}

//-------------------------------------------------------------------

bool 
flushDisk::read(void *a_void, 
				unsigned long a_pos) const
{
	operType = FLUSH_READ;
	
	if (!opened)
		open();
	
	if (fileType == REG_FILE || fileType == TMP_FILE)
	{
		a_pos *= size;		
		fseek(file,a_pos,SEEK_SET);
	}
	
	performXExec(preExec);
	
	///execute 
	if (fileType == REG_FILE || fileType == TMP_FILE)
		fread(a_void,size,1,file);
	else
	#ifndef FAST
		if (fileType == FIFO_FILE)
	#endif
			fgets((char *)a_void,size,file);
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
		}
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case ENOMEM:
			case EOVERFLOW:
				if (!persistant)
					close();				
				return false;
		}
	
	#endif
			
	performXExec(postExec);

	if (bufferize)
		buffer.assign((char *)a_void,size);
	
	return true;	
}

//-------------------------------------------------------------------

bool 
flushDisk::readString(std::string &a_str, 
				unsigned long a_pos) const
{
	register char *data = new char[size+1];
	memset(data,0,size);
	register bool result = this->read((void *)data,a_pos);
	a_str.assign(data,size);
	delete [] data;
	return result;
}

//-------------------------------------------------------------------

bool 
flushDisk::writeString(const std::string &a_buf, 
				unsigned long a_pos)
{	
	return this->write(a_buf.c_str(),a_pos);	
}

//-------------------------------------------------------------------

bool 
flushDisk::write(const void *const a_buf, 
				unsigned long a_pos)
{	
	operType = FLUSH_WRITE;
	if (!opened)
		open();
	
	std::string stringToWrite((char *)a_buf);
		
	if (normalize)
		tools::normalize(stringToWrite,size);
			
	if (bufferize)
		buffer.assign(stringToWrite);
	
	if (fileType == REG_FILE || fileType == TMP_FILE)
	{
		register size_t read_bytes;
		register unsigned long pos = a_pos*size;
		if (!over && !append)
		{		
			register char *t_buffer = new char[size*size_of_char];
			if (t_buffer == NULL)
				#ifndef NO_EX
					throw flushDiskEx(FLUSHDISK_MEMORY_OVER, this,__LINE__,__FILE__);	
				#else
					return false;
				#endif
			fseek(file,pos,SEEK_SET);
			read_bytes = fread(t_buffer,size,1,file);
			delete [] t_buffer;
		}
			
		if (!over && read_bytes != 0)
			return false;
		
		if (append)
			fseek(file,0,SEEK_END);
		else
			fseek(file,pos,SEEK_SET);
	}
	
	performXExec(preExec);
	
	///execute 
	if (fileType == REG_FILE || fileType == TMP_FILE)
		fwrite(stringToWrite.c_str(),size,1,file);
	else
	#ifndef FAST
		if (fileType == FIFO_FILE)
	#endif
			fputs(stringToWrite.c_str(),file);
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
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case ENOMEM:
			case EOVERFLOW:	
			case EROFS:
				if (!persistant)
					close();
				return false;				
		}		
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
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EAGAIN:
			case EINTR:
			case ENOSPC:
			case EFBIG:	
			case EROFS:
				if (!persistant)
					close();
				return false;				
		}		
	#endif
	
	performXExec(postExec);
		
	return true;	
}

//-------------------------------------------------------------------

bool
flushDisk::erase(unsigned long a_pos)
{
	register char *empty = new char;
	if (empty == NULL)
		#ifndef NO_EX
			throw flushDiskEx(FLUSHDISK_MEMORY_OVER, this,__LINE__,__FILE__);	
		#else
			return false;
		#endif		
	register bool ret = this->write(empty,a_pos);
	delete empty;
	return ret;
}

//-------------------------------------------------------------------

bool 
flushDisk::unlink(const std::string &path)
{
	register int status(0);
	struct stat st;
	if (stat(path.c_str(),&st) == -1)
	{
		#ifndef NO_EX
			switch (errno)
			{							
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			}	
		#else
			switch (errno)
			{
				case EIO:
				case EINTR:
				case EACCES:
				case ENAMETOOLONG:
					return false;				
			}		
		#endif				
	}
	if (S_ISDIR(st.st_mode))
		status = ::rmdir(path.c_str());
	else
		status = ::unlink(path.c_str());
	if (status == -1)
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);		
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif
	return true;
}

//-------------------------------------------------------------------

bool 
flushDisk::rename(const std::string &oldPath, 
				const std::string &newPath)
{
	if (::rename(oldPath.c_str(),newPath.c_str()) != -1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);		
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOENT:
			case ENOTDIR:
			case EROFS:
				return false;				
		}		
	#endif			
}

//-------------------------------------------------------------------


bool 
flushDisk::link(const std::string &oldPath, 
				const std::string &newPath)
{
	if (::link(oldPath.c_str(),newPath.c_str()) != -1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);
			case ENOTDIR:
			case EEXIST:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EEXIST:
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOTDIR:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif	
}

//-------------------------------------------------------------------

bool 
flushDisk::symlink(const std::string &oldPath, 
					const std::string &newPath, 
					bool force)
{
	if (force)
	{
		struct stat st;
		if (stat(newPath.c_str(),&st) != -1)
			if (st.st_mode != S_IFLNK)
				return false;
	}
	if (::symlink(oldPath.c_str(),newPath.c_str()) != -1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);
			case ENOTDIR:
			case EEXIST:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,&dummyEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EEXIST:
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOTDIR:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif		
}					

//-------------------------------------------------------------------

bool 
flushDisk::chown(const std::string &path, 
				int uid)
{
	if (::chown(path.c_str(),uid,-1)!=-1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);		
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif		
}				

//-------------------------------------------------------------------

bool 
flushDisk::chgrp(const std::string &path, 
				int gid)
{
	if (::chown(path.c_str(),-1,gid) != -1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif		
}
					
//-------------------------------------------------------------------

bool 
flushDisk::touch(const std::string &path,
				 int a_time)
{
	if (a_time==-1)
		a_time = time(NULL);
	utimbuf temp = {a_time, a_time};
	
	if (::utime(path.c_str(),&temp)!=-1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EINTR:
			case EACCES:
			case ENAMETOOLONG:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif		
}

//-------------------------------------------------------------------

bool 
flushDisk::mkdir(const std::string &path, 
				int permissions,
				bool force)
{
	if (::mkdir(path.c_str(),getPermission(permissions))!=-1)
		return true;
	if ( (errno == EEXIST) && force )
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif		
}

//-------------------------------------------------------------------

bool 
flushDisk::chmod(const std::string &path, int permissions)
{
	if (::chmod(path.c_str(),getPermission(permissions))!=-1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENOMEM:
			case ENOSPC:
			case EMFILE:
			case ENFILE:
			case ENAMETOOLONG:
			case EOVERFLOW:
			case ENOENT:
			case EROFS:
				return false;				
		}		
	#endif		
}

//-------------------------------------------------------------------

mode_t 
flushDisk::getPermission(int permission)
{
	register mode_t mode(0);
	
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

bool 
flushDisk::rmdir(const std::string &path)
{
	DIR *directory = opendir(path.c_str());
	
	if (directory == NULL)
	{
		#ifndef NO_EX
			switch (errno)
			{
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,&dummyEx,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,&dummyEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
				case ENOENT:
					throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,&dummyEx,__LINE__,__FILE__);
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,&dummyEx,__LINE__,__FILE__);
			}	
		#else
			switch (errno)
			{
				case EIO:
				case EINTR:
				case EACCES:
				case ENOMEM:
				case ENOSPC:
				case EMFILE:
				case ENFILE:
				case ENAMETOOLONG:
				case EOVERFLOW:
				case ENOENT:
				case EROFS:
					return false;				
			}		
		#endif			
	}
	dirent *dd;
	struct stat st;
	std::string attached;
	
	while ( (dd=readdir(directory)) != NULL)
	{
		if ( (strcmp(dd->d_name,".") == 0) || (strcmp(dd->d_name,"..") == 0))
			continue;
		attached.assign(path+FILE_DELIM+dd->d_name);
		if (stat(attached.c_str(),&st) == -1)
		#ifndef NO_EX
			switch (errno)
			{							
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
			}	
		#else
			switch (errno)
			{
				case EIO:
				case EINTR:
				case EACCES:
				case ENAMETOOLONG:
					return false;				
			}		
		#endif		
		if (S_ISDIR(st.st_mode))
			flushDisk::rmdir(attached.c_str());
		else
			flushDisk::unlink(attached.c_str());
	}
	flushDisk::unlink(path);
}

//-------------------------------------------------------------------

permissionModesEnum 
flushDisk::getPermissions(const std::string &path)
{
	struct stat st;
	if (stat(path.c_str(),&st) == -1)
	#ifndef NO_EX
		switch (errno)
		{							
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENAMETOOLONG:
				return (permissionModesEnum)-1;
		}		
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
	if (stat(path.c_str(),&st) == -1)
	#ifndef NO_EX
		switch (errno)
		{							
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,&dummyEx,__LINE__,__FILE__);
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,&dummyEx,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, &dummyEx,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,&dummyEx,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EACCES:
			case ENAMETOOLONG:
				return (flushDiskFileTypeEnum)-1;
		}		
	#endif
	switch (st.st_mode)
	{
		case S_IFREG:
			return REGULAR_FILE;
		case S_IFLNK:
			return SYMBOLIC_LINK;
		case S_IFDIR:
			return DIRECTORY;
		case S_IFSOCK:
			return UNIX_SOCKET;
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

