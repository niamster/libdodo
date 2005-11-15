/***************************************************************************
 *            flushSTD.cc
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
 
#include "flushSTD.h"

using namespace dodo;

#define FLUSHSTDIN 0
#define FLUSHSTDOUT 1

dodoBase * const 
flushSTD::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

flushSTD::flushSTD()
{
}

//-------------------------------------------------------------------

flushSTD::flushSTD(flushSTD &fd)
{
}

//-------------------------------------------------------------------

flushSTD::~flushSTD()
{
	close();
}

//-------------------------------------------------------------------

int 
flushSTD::addPostExec(inExec func, 
					void *data) const
{
	return _addPostExec(func, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

int 
flushSTD::addPreExec(inExec func, 
					void *data) const
{
	return _addPreExec(func, (dodoBase *)this, data);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSTD::close() const 
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
		if (fclose(file[FLUSHSTDIN]) != 0)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;
			#endif		
			
		if (fclose(file[FLUSHSTDOUT]) != 0)
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
flushSTD::open(const std::string &a_path) const
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_OPEN;
	#endif
	
	if (opened)
		close();
		
	#ifndef FLUSH_DISK_WO_XEXEC	
		performXExec(preExec);	
	#endif
				
	file[FLUSHSTDIN] = fopen(stdin,"r");
	file[FLUSHSTDOUT] = fopen(stdin,"w");
	
	if (file[FLUSHSTDIN] == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	if (file[FLUSHSTDOUT] == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_OPEN,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
flushSTD::read(void * const a_void) const
{
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_READ;
	#endif
	
	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(preExec);
	#endif
	
	///execute 
	fread(a_void,inSize,1,file[FLUSHSTDIN]);
	
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
flushSTD::readString(std::string &a_str) const
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
	
	this->read((void *)data);
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
flushSTD::writeString(const std::string &a_buf)
{	
	return this->write(a_buf.c_str());	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushSTD::write(const void *const a_buf)
{	
	#ifndef FLUSH_DISK_WO_XEXEC
		operType = FLUSHDISK_OPER_WRITE;
	#endif
	
	std::string stringToWrite((char *)a_buf);
		
	if (normalize)
		tools::normalize(stringToWrite,outSize);
			
	if (bufferize)
		buffer.assign(stringToWrite);
		
	#ifndef FLUSH_DISK_WO_XEXEC
		performXExec(preExec);
	#endif
	
	///execute 
	fwrite(stringToWrite.c_str(),outSize,1,file[FLUSHSTDOUT]);
				
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
flushSTD::flush()
{
	if (fflush(file[FLUSHSTDOUT]) != 0)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHDISK,FLUSHDISK_FLUSH,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);	
		#else
			return false;
		#endif	
}
