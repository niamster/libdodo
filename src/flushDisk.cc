/***************************************************************************
 *            flushDisk.cc
 *
 *  Wed Oct 8 01:44:18 2005
 *  Copyright  2005  Ni@m
 *  niam.niam@gmail.com
 ****************************************************************************/

/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 2.1 as published by
 *  the Free Software Foundation;
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <flushDisk.h>

using namespace dodo;

flushDisk::flushDisk(short type, 
					const std::string &a_path) : over(false),
												mode(OPENMODE_READ_WRITE), 
												fileType(type), 
												append(false),
												path(a_path)
{
}

//-------------------------------------------------------------------

flushDisk::flushDisk(flushDisk &fd)
{
}

//-------------------------------------------------------------------

flushDisk::~flushDisk()
{
	if (opened)
		fclose(file);
}

//-------------------------------------------------------------------

int
flushDisk::getInDescriptor() const
{
	if (!opened)
		return -1;
		
	return fileno(file);
}

//-------------------------------------------------------------------

int
flushDisk::getOutDescriptor() const
{
	if (!opened)
		return -1;
		
	return fileno(file);
}

//-------------------------------------------------------------------

#ifndef FLUSH_DISK_WO_XEXEC

	int 
	flushDisk::addPostExec(inExec func, 
						void *data)
	{
		return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHDISK, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushDisk::addPreExec(inExec func, 
						void *data)
	{
		return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHDISK, data);
	}
	
	//-------------------------------------------------------------------
	
	#ifdef DL_EXT
	
		int 
		flushDisk::addPostExec(const std::string &module, 
							void *data,
							void *toInit)
		{
			return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data, toInit);
		}
		
		//-------------------------------------------------------------------
		
		int 
		flushDisk::addPreExec(const std::string &module, 
							void *data,
							void *toInit)
		{
			return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data, toInit);
		}
		
		//-------------------------------------------------------------------
		
		xexecCounts 
		flushDisk::addExec(const std::string &module, 
							void *data,
							void *toInit)
		{
			return _addExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data, toInit);
		}
	
	#endif

#endif

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushDisk::close() 
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_CLOSE;
	#endif

	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(preExec);
	#endif
			
	if (opened)
	{			
		if (fclose(file) != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
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
flushDisk::open(const std::string &a_path)
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_OPEN;
		performXExec(preExec);	
	#endif

	if (a_path.size() != 0 && strcmp(a_path.c_str(),path.c_str()) != 0)
		path = a_path;
		
	if (opened)
		close();
	
	if (fileType == FILETYPE_TMP_FILE)
		file = tmpfile();
	else
	{
		if (path.size() == 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__,path);
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
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
					#else
						return false;							
					#endif
			}
			else
				exists = true;
			
			if (fileType == FILETYPE_FIFO_FILE)
			{
				if (exists && !S_ISFIFO(st.st_mode))
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__,path);
					#else
						return false;
					#endif
				if (!exists)
					if (mkfifo(path.c_str(),flushDiskTools::getPermission(FILE_PERM)) == -1)
						#ifndef NO_EX
							throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
						#else
							return false;		
						#endif					
			}
			else
			{
				if ( (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE || FILETYPE_CHAR_FILE) && exists && !S_ISREG(st.st_mode) && !S_ISCHR(st.st_mode))
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_LIBDODO,FLUSHDISK_WRONG_FILENAME,FLUSHDISK_WRONG_FILENAME_STR,__LINE__,__FILE__,path);
					#else
						return false;
					#endif
			}
							
			switch (mode)
			{
				case OPENMODE_READ_WRITE:
				
					file = fopen(path.c_str(),"r+");
					if (file == NULL)
						file = fopen(path.c_str(),"w+");
						
					break;
					
				case OPENMODE_READ_WRITE_TRUNCATE:
				
					file = fopen(path.c_str(),"w+");
					
					break;
					
				case OPENMODE_APPEND:
				
					file = fopen(path.c_str(),"a+");
					append = true;
					
					break;
					
				case OPENMODE_READ_ONLY:
				default:
				
					file = fopen(path.c_str(),"r");			
			}
		}
	}
	
	if (file == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
		#else
			return false;
		#endif
	
	flushDiskTools::chmod(path,FILE_PERM);
	
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
				unsigned long a_pos)
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_READ;
		performXExec(preExec);
	#endif
	
	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
		if (fseek(file,a_pos*inSize,SEEK_SET) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
			#else
				return false;
			#endif
	
	memset(a_void,'\0',inSize);
	
	errno = 0; 
	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
		fread(a_void,inSize,1,file);
	else
		#ifndef FAST
			if (fileType == FIFO_FILE)
		#endif
				fgets(a_void,inSize+1,file);
	
	#ifndef NO_EX
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOMEM:
			case ENXIO:
			
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
		}	
	#else			
		switch (errno)
		{
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOMEM:
			case ENXIO:
			
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
				unsigned long a_pos)
{
	register char *data = new char[inSize+1];

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
	buffer.assign(a_buf, outSize);
				
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_WRITE;
		performXExec(preExec);
	#endif
	
	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
	{
		a_pos *= outSize;
		
		if (!append)
		{	
			if (!over)
			{
				register size_t read_bytes(0);
				char *t_buf = new char[outSize];
				
				if (fseek(file,a_pos,SEEK_SET) == -1)
					#ifndef NO_EX
					{
						delete [] t_buf;
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
					}
					#else
					{
						delete [] t_buf;
						return false;
					}
					#endif
					
				read_bytes = fread(t_buf,outSize,1,file);
				
				delete [] t_buf;
				
				if (read_bytes != 0)
					#ifndef NO_EX
						throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_LIBDODO,FLUSHDISK_CANNOT_OVEWRITE,FLUSHDISK_CANNOT_OVEWRITE_STR,__LINE__,__FILE__,path);
					#else
						return false;
					#endif
			}	
			
			if (fseek(file,a_pos,SEEK_SET) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
				#else
					return false;
				#endif
		}
		else
			if (fseek(file,0,SEEK_END) == -1)
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
				#else
					return false;
				#endif
	}
	
	errno = 0; 
	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
		fwrite(buffer.c_str(),outSize,1,file);
	else
		#ifndef FAST
			if (fileType == FILETYPE_FIFO_FILE)
		#endif
				fputs(buffer.c_str(),file);
			
	#ifndef NO_EX
		switch (errno)
		{
			case EFBIG:
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOSPC:
			case EPIPE:
			case ENOMEM:
			case ENXIO:
			
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
		}	
	#else			
		switch (errno)
		{
			case EFBIG:
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOSPC:
			case EPIPE:
			case ENOMEM:
			case ENXIO:
			
				return false;
		}
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
	register char *empty = new char[outSize];
	
	memset(empty,0,outSize);

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->write(empty,a_pos);
	
	delete [] empty;
	
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
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_FLUSH,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);	
		#else
			return false;
		#endif
			
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

std::string 
flushDisk::getPath() const
{
	return path;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushDisk::readStream(char * const a_void, 
				unsigned long a_pos)
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_READSTREAM;
		performXExec(preExec);
	#endif
	
	if (fileType == FILETYPE_REG_FILE || fileType == FILETYPE_TMP_FILE)
	{
		if (fseek(file,0,SEEK_SET) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READSTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
			#else
				return false;
			#endif
			
		for (register unsigned long i(0);i<a_pos;++i)
		{
			if (fgets(a_void,inSize,file)==NULL)
			{
				#ifndef NO_EX
					switch (errno)
					{
						case EIO:
						case EINTR:
						case EBADF:
						case EOVERFLOW:
						case ENOMEM:
						case ENXIO:
						
							throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READSTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
					}	
				#else			
					switch (errno)
					{
						case EIO:
						case EINTR:
						case EBADF:
						case EOVERFLOW:
						case ENOMEM:
						case ENXIO:
						
							return false;
					}
				#endif
				
				#ifndef NO_EX
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READSTREAM,ERR_LIBDODO,FLUSHDISK_FILE_IS_SHORTER_THAN_GIVEN_POSITION,FLUSHDISK_FILE_IS_SHORTER_THAN_GIVEN_POSITION_STR,__LINE__,__FILE__,path);
				#else
					return false;
				#endif				
			}
		}
	}
	
	memset(a_void,'\0',inSize);
	 
	if (fgets(a_void,inSize,file)==NULL)
		#ifndef NO_EX
			switch (errno)
			{
				case EIO:
				case EINTR:
				case EBADF:
				case EOVERFLOW:
				case ENOMEM:
				case ENXIO:
				
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_READSTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
			}	
		#else			
			switch (errno)
			{
				case EIO:
				case EINTR:
				case EBADF:
				case EOVERFLOW:
				case ENOMEM:
				case ENXIO:
				
					return false;
			}
		#endif
	
	buffer.assign(a_void);
	
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
flushDisk::readStreamString(std::string &a_str, 
				unsigned long a_pos)
{
	register char *data = new char[inSize+1];

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->readStream(data,a_pos);
	a_str.assign(data);
	
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
flushDisk::writeStreamString(const std::string &a_buf)
{	
	return this->writeStream(a_buf.c_str());	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushDisk::writeStream(const char *const a_buf)
{
	buffer.assign(a_buf);
				
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_WRITESTREAM;
		performXExec(preExec);
	#endif
	
	if (fseek(file,0,SEEK_END) == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITESTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
			#else
				return false;
			#endif

	if (buffer.size()>outSize)
		buffer.resize(outSize);
	 
	if (fputs(buffer.c_str(),file) < 0)
		#ifndef NO_EX
			switch (errno)
			{
				case EFBIG:
				case EIO:
				case EINTR:
				case EBADF:
				case EOVERFLOW:
				case ENOSPC:
				case EPIPE:
				case ENOMEM:
				case ENXIO:
				
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITESTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
			}	
		#else			
			switch (errno)
			{
				case EFBIG:
				case EIO:
				case EINTR:
				case EBADF:
				case EOVERFLOW:
				case ENOSPC:
				case EPIPE:
				case ENOMEM:
				case ENXIO:
				
					return false;
			}
		#endif

	if (fputc('\n',file) < 0)
		#ifndef NO_EX
			switch (errno)
			{
				case EFBIG:
				case EIO:
				case EINTR:
				case EBADF:
				case EOVERFLOW:
				case ENOSPC:
				case EPIPE:
				case ENOMEM:
				case ENXIO:
				
					throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_WRITESTREAM,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__,path);
			}	
		#else			
			switch (errno)
			{
				case EFBIG:
				case EIO:
				case EINTR:
				case EBADF:
				case EOVERFLOW:
				case ENOSPC:
				case EPIPE:
				case ENOMEM:
				case ENXIO:
				
					return false;
			}
		#endif
		
	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(postExec);
	#endif
			
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------
