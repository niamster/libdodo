/***************************************************************************
 *            flushDisk.cc
 *
 *  Wed Feb 23 01:44:18 2005
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

flushDisk::flushDisk(flushDiskFileTypeEnum type, 
					const std::string &a_path, 
					bool a_pers) : persistant(a_pers), 
									size(1), 
									opened(false), 
									mode(READ_WRITE), 
									over(false), 
									fileType(type),
									bufferize(false),
									path(a_path)
{
	if (persistant)
		open();		
}

//-------------------------------------------------------------------

flushDisk::flushDisk(bool a_pers) : persistant(a_pers), 
									size(1), 
									opened(false), 
									mode(READ_WRITE), 
									over(false),
									bufferize(false),
									fileType(TMP_FILE)
{
	if (persistant)
		open();		
}

//-------------------------------------------------------------------

flushDisk::~flushDisk()
{
	close();
}

//-------------------------------------------------------------------

bool
flushDisk::close()
{
	if (opened)
	{
		register int ret = fclose(file);
		#ifndef NO_EX
			switch (ret)
			{
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
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
flushDisk::open()
{
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
			if (fileType == FIFO_FILE)
			{
				struct stat st;
				if (stat(path.c_str(),&st)==-1)
				{
					#ifndef NO_EX
						switch (errno)
						{
							case EACCES:
								throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
							case EIO:
								throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
							case EINTR:
								throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
							case ENAMETOOLONG:
								throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
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
					umask(0);
					if (mknod(path.c_str(),S_IFIFO|0666,0) == -1)
					#ifndef NO_EX
						switch (errno)
						{
							case EACCES:
								throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
							case EIO:
								throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
							case EINTR:
								throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
							case ENOSPC:
							case ENOMEM:
								throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
							case EMFILE:
							case ENFILE:
								throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
							case ENAMETOOLONG:
								throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
							case ENOENT:
								throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);	
							case EROFS:
								throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);		
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
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);		
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
				unsigned long a_pos)
{
	a_pos *= size;
	if (!opened)
		open();
	fseek(file,a_pos,SEEK_SET);

	size_t read_bytes = fread(a_void,size,1,file);

	if (bufferize)
		buffer.assign((char *)a_void);
	
	if (!persistant)
		close();
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
	if (read_bytes == 0)
		return false;
	return true;	
}

//-------------------------------------------------------------------

bool 
flushDisk::read(std::string &a_str, 
				unsigned long a_pos)
{
	return this->read((void *)a_str.c_str(),a_pos);
}

//-------------------------------------------------------------------

bool 
flushDisk::write(const std::string &a_buf, 
				unsigned long a_pos)
{		
	return this->write(a_buf.c_str(),a_pos);	
}

//-------------------------------------------------------------------

bool 
flushDisk::write(const void *const a_buf, 
				unsigned long a_pos)
{		
	if (!opened)
		open();
	if (append)
		fseek(file,0,SEEK_END);
	else
	{
		a_pos *= size;
		fseek(file,a_pos,SEEK_SET);
	}
	
	register size_t read_bytes;
	
	if (bufferize)
		buffer.assign((char *)a_buf);
	
	if (!over)
	{		
		register char *t_buffer = new char[size*size_of_char];
		if (t_buffer == NULL)
			#ifndef NO_EX
				throw flushDiskEx(FLUSHDISK_MEMORY_OVER, this,__LINE__,__FILE__);	
			#else
				return false;
			#endif
		read_bytes = fread(t_buffer,size,1,file);
		delete [] t_buffer;
	}
	if (!over && read_bytes != 0)
	{
		if (!persistant)
			close();
		return false;
	}
	if (append)
		fseek(file,0,SEEK_END);
	else
	{
		a_pos *= size;
		fseek(file,a_pos,SEEK_SET);
	}
	read_bytes = fwrite(a_buf,size,1,file);

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
	if (!persistant)
		close();
	if (read_bytes == 0)
		return false;
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
					throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			}	
		#else
			switch (errno)
			{
				case EIO:
				case EINTR:
				case EACCES:
				case ENAMETOOLONG:
					if (!persistant)
						close();
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
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);		
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
				if (!persistant)
					close();
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
	if (rename(oldPath.c_str(),newPath.c_str()) != -1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);		
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
				if (!persistant)
					close();
				return false;				
		}		
	#endif			
}

//-------------------------------------------------------------------


bool 
flushDisk::link(const std::string &oldPath, 
				const std::string &newPath)
{
	if (link(oldPath.c_str(),newPath.c_str()) != -1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);
			case ENOTDIR:
			case EEXIST:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,this,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
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
				if (!persistant)
					close();
				return false;				
		}		
	#endif	
}

//-------------------------------------------------------------------

bool 
flushDisk::symlink(const std::string &oldPath, 
					const std::string &newPath)
{
	if (symlink(oldPath.c_str(),newPath.c_str()) != -1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);
			case ENOTDIR:
			case EEXIST:
					throw flushDiskEx(FLUSHDISK_WRONG_FILE_NAME,this,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
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
				if (!persistant)
					close();
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
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);	
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);		
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
				if (!persistant)
					close();
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
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
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
				if (!persistant)
					close();
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
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
		}	
	#else
		switch (errno)
		{
			case EINTR:
			case EACCES:
			case ENAMETOOLONG:
			case ENOENT:
			case EROFS:
				if (!persistant)
					close();
				return false;				
		}		
	#endif		
}

//-------------------------------------------------------------------

bool 
flushDisk::mkdir(const std::string &path, int permissions)
{
	if (::mkdir(path.c_str(),getPermission(permissions))!=-1)
		return true;
	#ifndef NO_EX
		switch (errno)
		{
			case EACCES:
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
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
				if (!persistant)
					close();
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
				throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
			case EIO:
				throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
			case EINTR:
				throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
			case ENOSPC:
			case ENOMEM:
				throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
			case EMFILE:
			case ENFILE:
				throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
			case ENAMETOOLONG:
				throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			case ENOENT:
				throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);
			case EROFS:
				throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
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
				if (!persistant)
					close();
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
	if ((ALL_READ_ACCESS & permission) == ALL_READ_ACCESS)
		mode |= S_IROTH;
			
	if ((OWNER_WRITE_ACCESS & permission) == OWNER_WRITE_ACCESS)
		mode |= S_IWUSR;
	if ((GROUP_WRITE_ACCESS & permission) == GROUP_WRITE_ACCESS)
		mode |= S_IWGRP;
	if ((ALL_WRITE_ACCESS & permission) == ALL_WRITE_ACCESS)
		mode |= S_IWOTH;
			
	if ((STICKY_ACCESS & permission) == STICKY_ACCESS)
		mode |= S_IRUSR;
			
	if ((OWNER_EXECUTE_ACCESS & permission) == OWNER_EXECUTE_ACCESS)
		mode |= S_IXUSR;
	if ((GROUP_EXECUTE_ACCESS & permission) == GROUP_EXECUTE_ACCESS)
		mode |= S_IXGRP;
	if ((ALL_EXECUTE_ACCESS & permission) == ALL_EXECUTE_ACCESS)
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
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
				case EIO:
					throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
				case ENOSPC:
				case ENOMEM:
					throw flushDiskEx(FLUSHDISK_NOT_ENOUGH_FREE_SPACE,this,__LINE__,__FILE__);
				case EMFILE:
				case ENFILE:
					throw flushDiskEx(FLUSHDISK_TOO_MANY_OPEN_FILE,this,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
				case ENOENT:
					throw flushDiskEx(FLUSHDISK_NO_SUCH_FILE,this,__LINE__,__FILE__);
				case EROFS:
					throw flushDiskEx(FLUSHDISK_READ_ONLY_FS,this,__LINE__,__FILE__);
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
					if (!persistant)
						close();
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
					throw flushDiskEx(FLUSHDISK_IOERROR,this,__LINE__,__FILE__);
				case EACCES:
					throw flushDiskEx(FLUSHDISK_ACCESS_DENIED,this,__LINE__,__FILE__);
				case EINTR:
					throw flushDiskEx(FLUSHDISK_INTERRUPTED, this,__LINE__,__FILE__);
				case ENAMETOOLONG:
					throw flushDiskEx(FLUSHDISK_FILENAME_TOO_LONG,this,__LINE__,__FILE__);
			}	
		#else
			switch (errno)
			{
				case EIO:
				case EINTR:
				case EACCES:
				case ENAMETOOLONG:
					if (!persistant)
						close();
					return false;				
			}		
		#endif		
		if (S_ISDIR(st.st_mode))
			this->rmdir(attached.c_str());
		else
			this->unlink(attached.c_str());
	}
	this->unlink(path);
}