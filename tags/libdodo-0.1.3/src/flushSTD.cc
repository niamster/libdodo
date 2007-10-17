/***************************************************************************
 *            flushSTD.cc
 *
 *  Tue Nov 15 21:19:57 2005
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
 
#include <flushSTD.h>

using namespace dodo;

flushSTD::flushSTD() : inSTDBuffer(STD_INSIZE),
						outSTDBuffer(STD_OUTSIZE),
						err(false),
						blocked(true)
{
}

//-------------------------------------------------------------------

flushSTD::flushSTD(flushSTD &fd)
{
}

//-------------------------------------------------------------------

flushSTD::~flushSTD()
{
}

//-------------------------------------------------------------------

#ifndef FLUSH_STD_WO_XEXEC

	int 
	flushSTD::addPostExec(inExec func, 
						void *data) const
	{
		return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHSTD, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushSTD::addPreExec(inExec func, 
						void *data) const
	{
		return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHSTD, data);
	}
	
	//-------------------------------------------------------------------
	
	#ifdef DL_EXT
	
		int 
		flushSTD::addPostExec(const std::string &module, 
							void *data) const
		{
			return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHSTD, data);
		}
		
		//-------------------------------------------------------------------
		
		xexecCounts 
		flushSTD::addExec(const std::string &module, 
							void *data) const
		{
			return _addExec(module, (void *)this, XEXECOBJ_FLUSHSTD, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		flushSTD::addPreExec(const std::string &module, 
							void *data) const
		{
			return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHSTD, data);
		}
	
	#endif

#endif

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSTD::read(char * const a_void) const
{
	#ifndef FLUSH_STD_WO_XEXEC
		operType = FLUSHSTD_OPER_READ;
		performXExec(preExec);
	#endif
	
	///execute 
	iter = inSize/inSTDBuffer;
	rest = inSize%inSTDBuffer;
	
	register long recieved(0);
	
	for (register long i=0;i<iter;++i)
	{
		if (fread(a_void+recieved,inSTDBuffer,1,stdin)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			
		recieved += inSTDBuffer;
	}
	
	if (rest>0)
		if (fread(a_void+recieved,rest,1,stdin)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			
	#ifndef FLUSH_STD_WO_XEXEC		
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
flushSTD::readString(std::string &a_str) const
{
	register char *data = new char[inSize+1];
		
	memset(data,0,inSize);

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->read(data);
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
flushSTD::write(const char *const aa_buf)
{
	register long oldOutSize = outSize;		
	if (autoOutSize)
		outSize = strlen(aa_buf);
		
	buffer.assign(aa_buf, outSize);		
		
	#ifndef FLUSH_STD_WO_XEXEC
		operType = FLUSHSTD_OPER_WRITE;
		performXExec(preExec);
	#endif
		
	if (autoOutSize)
		outSize = buffer.size();
	
	///execute 
	iter = outSize/outSTDBuffer;
	rest = outSize%outSTDBuffer;
	
	register long sent(0);
	
	desc = stdout;
	if (err)
		desc = stderr;
	
	for (register long i=0;i<iter;++i)
	{
		if (fwrite(buffer.c_str()+sent,outSTDBuffer,1,desc)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			
		sent += outSTDBuffer;
	}

	if (rest>0)
		if (fwrite(buffer.c_str()+sent,rest,1,desc)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			
	#ifndef FLUSH_STD_WO_XEXEC
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
flushSTD::flush()
{
	desc = stdout;
	if (err)
		desc = stderr;

    if (fflush(desc) != 0)
        #ifndef NO_EX
                throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_FLUSH,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
        #else
                return false;
        #endif
}

//-------------------------------------------------------------------

__connInfo 
flushSTD::inputterInfo()
{
	__connInfo info;
	
	struct sockaddr sa;
	
	register socklen_t len = sizeof(sockaddr_in6);
	
	if (::getpeername(1,&sa,&len)==-1)
		#ifndef NO_EX
		{
			if (errno!=ENOTSOCK)
				throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_INPUTTERINFO,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			else
				return info;
		}
		#else
			return info;
		#endif

	switch (len)
	{
		case sizeof(sockaddr_in):
		{
			char temp[15];
			sockaddr_in *sa4;
			sa4 = (sockaddr_in *)&sa;
			if (inet_ntop(AF_INET,&(sa4->sin_addr),temp,15) != NULL)
				info.host.assign(temp);
			info.port = ntohs(sa4->sin_port);
			return info;
		}
			
		case sizeof(sockaddr_in6):
		{
				char temp[INET6_ADDRSTRLEN];
				sockaddr_in6 *sa6;
				sa6 = (sockaddr_in6 *)&sa6;
				if (inet_ntop(AF_INET6,&(sa6->sin6_addr),temp,INET6_ADDRSTRLEN) != NULL)
					info.host.assign(temp);
				info.port = ntohs(sa6->sin6_port);
			return info;
		}
			
		default:
			return info;
			
	}
}

//-------------------------------------------------------------------

bool 
flushSTD::isBlocked()
{
	return blocked;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSTD::block(bool flag)
{
	int block[3] = {O_NONBLOCK, O_NONBLOCK, O_NONBLOCK};
	
	if (flag)
	{
		block[0] = fcntl(0,F_GETFL);
		if (block[0] == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;		
			#endif		
		
		block[0] &= ~O_NONBLOCK;
			
		block[1] = fcntl(1,F_GETFL);
		if (block[1] == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;		
			#endif
		
		block[1] &= ~O_NONBLOCK;
				
		block[2] = fcntl(2,F_GETFL);
		if (block[2] == -1)
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			#else
				return false;		
			#endif	
								
		block[2] &= ~O_NONBLOCK;
	}
	
	if (fcntl(0,F_SETFL,block[0])==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif		
	
	if (fcntl(1,F_SETFL,block[1])==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif		
		
	if (fcntl(2,F_SETFL,block[2])==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
		
	blocked = flag;
	
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
flushSTD::readStream(char * const a_void) const
{
	#ifndef FLUSH_STD_WO_XEXEC
		operType = FLUSHSTD_OPER_READSTREAM;
		performXExec(preExec);
	#endif

	///execute
	if (fgets(a_void,inSTDBuffer+1,stdin)==NULL)
		#ifndef NO_EX
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:
					throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_READ,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
		
	buffer.assign(a_void);
			
	#ifndef FLUSH_STD_WO_XEXEC		
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
flushSTD::readStreamString(std::string &a_str) const
{
	register char *data = new char[inSTDBuffer+1];
		
	memset(data,0,inSTDBuffer);

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->readStream(data);
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
flushSTD::writeStreamString(const std::string &a_buf)
{	
	return this->writeStream(a_buf.c_str());	
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushSTD::writeStream(const char *const aa_buf)
{
	buffer.assign(aa_buf);		
		
	#ifndef FLUSH_STD_WO_XEXEC
		operType = FLUSHSTD_OPER_WRITESTREAM;
		performXExec(preExec);
	#endif
	
	///execute 
	desc = stdout;
	if (err)
		desc = stderr;

	int outSize = strlen(aa_buf);
	
	///execute 
	iter = outSize/outSTDBuffer;
	rest = outSize%outSTDBuffer;
	
	register long sent(0);
	
	desc = stdout;
	if (err)
		desc = stderr;
	
	register char buff[outSTDBuffer+1];
	
	for (register long i=0;i<iter;++i)
	{
		strncpy(buff,buffer.c_str()+sent,outSTDBuffer);
		buff[outSTDBuffer] = '\0';
		
		if (fputs(buff,desc)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			
		sent += outSTDBuffer;
	}

	if (rest>0)
	{		
		strncpy(buff,buffer.c_str()+sent,rest);
		buff[rest] = '\0';
		
		if (fputs(buff,desc)==0)
			#ifndef NO_EX
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:
						throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_WRITE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
			
	#ifndef FLUSH_STD_WO_XEXEC
		performXExec(postExec);
	#endif
			
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------
