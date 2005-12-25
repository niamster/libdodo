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

#ifndef FLUSH_DISK_WO_XEXEC

	int 
	flushDisk::addPostExec(inExec func, 
						void *data) const
	{
		return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHDISK, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushDisk::addPreExec(inExec func, 
						void *data) const
	{
		return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHDISK, data);
	}
	
	//-------------------------------------------------------------------
	
	#ifdef DL_EXT
	
		int 
		flushDisk::addPostExec(const std::string &module, 
							void *data) const
		{
			return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		flushDisk::addPreExec(const std::string &module, 
							void *data) const
		{
			return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data);
		}
		
		//-------------------------------------------------------------------
		
		xexecCounts 
		flushDisk::addExec(const std::string &module, 
							void *data) const
		{
			return _addExec(module, (void *)this, XEXECOBJ_FLUSHDISK, data);
		}
	
	#endif

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
					if (mkfifo(path.c_str(),flushDiskTools::getPermission(FILE_PERM)) == -1)
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

std::string 
flushDisk::getPath() const
{
	return path;
}
//-------------------------------------------------------------------

