/***************************************************************************
 *            flushSocketOptions.cc
 *
 *  Thu Sep 20 01:43:24 2005
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
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <flushSocketOptions.h>

using namespace dodo;

flushSocketOptions::flushSocketOptions(socketProtoFamilyEnum a_family, 
										socketTransferTypeEnum a_type): family(a_family),
																	type(a_type),
																	lingerOpts(SOCKET_LINGER_OPTION),
																	lingerSeconds(SOCKET_LINGER_PERIOD),
																	inTimeout(RECIEVE_TIMEOUT),
																	outTimeout(SEND_TIMEOUT),
																	inSocketBuffer(SOCKET_INSIZE),
																	outSocketBuffer(SOCKET_OUTSIZE),
																	socket(-1),
																	blocked(true)
{
}

//-------------------------------------------------------------------

flushSocketOptions::flushSocketOptions(): lingerOpts(SOCKET_LINGER_OPTION),
										lingerSeconds(SOCKET_LINGER_PERIOD),
										inTimeout(RECIEVE_TIMEOUT),
										outTimeout(SEND_TIMEOUT),
										inSocketBuffer(SOCKET_INSIZE),
										outSocketBuffer(SOCKET_OUTSIZE),
										socket(-1),
										blocked(false)
{
}

//-------------------------------------------------------------------

flushSocketOptions::~flushSocketOptions()
{
}

//-------------------------------------------------------------------

bool 
flushSocketOptions::isBlocked()
{
	return blocked;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketOptions::block(bool flag)
{
	int block = O_NONBLOCK;
	
	block = fcntl(socket,F_GETFL);
	if (block == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif		
	
	if (flag)
		block &= ~O_NONBLOCK;
	else
		block |= O_NONBLOCK;
	
	if (fcntl(socket,F_SETFL,block)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_BLOCK,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
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
flushSocketOptions::setInBufferSize(int bytes)
{
	if (socket == -1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETINBUFFERSIZE,ERR_LIBDODO,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#else
			return false;
		#endif
	
	inSocketBuffer = bytes;
	
	if (setsockopt(socket,SOL_SOCKET,SO_RCVBUF,&inSocketBuffer,sizeof(long))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETINBUFFERSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------

int
flushSocketOptions::getInBufferSize() const
{
	return inSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketOptions::setOutBufferSize(int bytes)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETOUTBUFFERSIZE,ERR_LIBDODO,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	outSocketBuffer = bytes;
	
	if (setsockopt(socket,SOL_SOCKET,SO_SNDBUF,&outSocketBuffer,sizeof(long))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETOUTBUFFERSIZE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif				
}

//-------------------------------------------------------------------

int
flushSocketOptions::getOutBufferSize() const
{
	return outSocketBuffer;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif 
flushSocketOptions::setInTimeout(unsigned long microseconds)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETINTIMEOUT,ERR_LIBDODO,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif

	inTimeout = microseconds;

	timeval val;
	val.tv_sec = inTimeout/100;
	val.tv_usec = inTimeout%100;
	
	if (setsockopt(socket,SOL_SOCKET,SO_RCVTIMEO,&val,sizeof(val))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETINTIMEOUT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;	
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}


//-------------------------------------------------------------------

unsigned long 
flushSocketOptions::getInTimeout() const
{
	return inTimeout;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketOptions::setOutTimeout(unsigned long microseconds)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETOUTTIMEOUT,ERR_LIBDODO,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif

	outTimeout = microseconds;

	timeval val;
	val.tv_sec = outTimeout/100;
	val.tv_usec = outTimeout%100;
	
	if (setsockopt(socket,SOL_SOCKET,SO_SNDTIMEO,&val,sizeof(val))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETOUTTIMEOUT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;		
		#endif	
		
	#ifdef NO_EX
		return true;
	#endif	
}


//-------------------------------------------------------------------

unsigned long 
flushSocketOptions::getOutTimeout() const
{
	return outTimeout;
}

//-------------------------------------------------------------------

bool
flushSocketOptions::getSocketOpts(int option) const
{
	if  ( (option&socketOpts) == option)
		return true;
	return false;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketOptions::setSockOption(socketOptionsEnum option, 
							bool flag)
{
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETSOCKOPT,ERR_LIBDODO,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	register int sockFlag(1);
	
	if (!flag)
		sockFlag = 0;
	
	register int real_option(0);
	
	switch (option)
	{
		case SOCKET_KEEP_ALIVE:
			real_option = SO_KEEPALIVE;
			break;
		case SOCKET_REUSE_ADDRESS:
			real_option = SO_REUSEADDR;
			break;
		case SOCKET_DONOT_USE_GATEWAY:
			real_option = SO_DONTROUTE;
			break;
		case SOCKET_BROADCAST:
			real_option = SO_BROADCAST;
			break;
		case SOCKET_OOB_INLINE:
			real_option = SO_OOBINLINE;
			break;
		#ifdef SO_REUSEPORT	
			case SOCKET_REUSE_PORT:
				real_option = SO_REUSEPORT;
				break;
		#endif
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETSOCKOPT,ERR_LIBDODO,FLUSHSOCKETOPTIONS_WRONG_PARAMETHER,FLUSHSOCKETOPTIONS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;			
			#endif	
	}
	
	if (setsockopt(socket,SOL_SOCKET,real_option,&sockFlag,sizeof(int))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETSOCKOPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;	
		#endif	
	
	if (!flag)
		removeF(socketOpts,1<<option);
	else
		addF(socketOpts,1<<option);	
		
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
flushSocketOptions::setLingerSockOption(socketLingerOption option,
									int seconds)
{	
	if (socket == -1)
		#ifdef NO_EX
			return false;
		#else
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETLINGERSOCKOPT,ERR_LIBDODO,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED,FLUSHSOCKETOPTIONS_NO_SOCKET_CREATED_STR,__LINE__,__FILE__);
		#endif
	
	linger lin;
	
	switch (option)
	{
		case SOCKET_GRACEFUL_CLOSE:
			lin.l_onoff = 0;
			break;
		case SOCKET_HARD_CLOSE:
			lin.l_onoff = 1;
			lin.l_linger = 0;
			break;
		case SOCKET_WAIT_CLOSE:
			lin.l_onoff = 1;
			lin.l_linger = seconds;
			break;
		default:
			#ifndef NO_EX
				throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETLINGERSOCKOPT,ERR_LIBDODO,FLUSHSOCKETOPTIONS_WRONG_PARAMETHER,FLUSHSOCKETOPTIONS_WRONG_PARAMETHER_STR,__LINE__,__FILE__);
			#else
				return false;			
			#endif	
	}

	if (setsockopt(socket,SOL_SOCKET,SO_LINGER,&lin,sizeof(linger))==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS_SETLINGERSOCKOPT,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;	
		#endif
		
	lingerOpts = option;
	lingerSeconds = seconds;		
	
	#ifdef NO_EX
		return true;
	#endif	
}

//-------------------------------------------------------------------

socketLingerOption 
flushSocketOptions::getLingerOption() const
{
	return lingerOpts;
}

//-------------------------------------------------------------------

int 
flushSocketOptions::getLingerPeriod() const
{
	return lingerSeconds;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketOptions::_close(int socket)
{
	if (::shutdown(socket,SHUT_RDWR)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS__CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif			
	
	if (::close(socket)==-1)
		#ifndef NO_EX
			throw baseEx(ERRMODULE_FLUSHSOCKETOPTIONS,FLUSHSOCKETOPTIONS__CLOSE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
		#else
			return false;
		#endif
		
	#ifdef NO_EX
		return true;
	#endif			
}

//-------------------------------------------------------------------
