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
 
#include "flushSTD.h"

using namespace dodo;

dodoBase * const 
flushSTD::getSelf()
{
	return dynamic_cast<dodoBase *>(this);
}

//-------------------------------------------------------------------

flushSTD::flushSTD() : inSTDBuffer(STD_INSIZE),
						outSTDBuffer(STD_OUTSIZE)
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
flushSTD::read(char * const a_void) const
{
	#ifndef FLUSH_STD_WO_XEXEC
		operType = FLUSHSTD_OPER_READ;
	#endif
	
	#ifndef FLUSH_STD_WO_XEXEC
		performXExec(preExec);
	#endif
	
	///execute 
	register long iter = inSize/inSTDBuffer, rest = inSize%inSTDBuffer;
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
	if (data == NULL)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_READSTRING,ERR_LIBDODO,FLUSHSTD_MEMORY_OVER,FLUSHSTD_MEMORY_OVER_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
		
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
	#ifndef FLUSH_STD_WO_XEXEC
		operType = FLUSHSTD_OPER_WRITE;
	#endif
		
	register long oldOutSize = outSize;		
	if (autoOutSize)
		outSize = strlen(aa_buf);
				
	buffer.assign(aa_buf, outSize);		
		
	#ifndef FLUSH_STD_WO_XEXEC
		performXExec(preExec);
	#endif
		
	if (autoOutSize)
		outSize = buffer.size();
	
	///execute 
	register long iter = outSize/outSTDBuffer, rest = outSize%outSTDBuffer;
	register long sent(0);
	
	for (register long i=0;i<iter;++i)
	{
		if (fwrite(buffer.c_str()+sent,outSTDBuffer,1,stdout)==0)
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
		if (fwrite(buffer.c_str()+sent,rest,1,stdout)==0)
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
        if (fflush(stdout) != 0)
                #ifndef NO_EX
                        throw baseEx(ERRMODULE_FLUSHSTD,FLUSHSTD_FLUSH,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
                #else
                        return false;
                #endif
}

//-------------------------------------------------------------------
