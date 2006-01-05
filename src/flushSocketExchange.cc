/***************************************************************************
 *            flushSocketExchange.cc
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
 
#include <flushSocketExchange.h>

using namespace dodo;

__initialAccept::__initialAccept() : socket(-1), 
									family((socketProtoFamilyEnum)-1), 
									type((socketTransferTypeEnum)-1)
{
}

//-------------------------------------------------------------------

__initialAccept::__initialAccept(__initialAccept &init) : socket(init.socket), 
														family(init.family), 
														type(init.type)
{
	init.socket = -1;
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange(flushSocketExchange &fse)
{
	socket = fse.socket;
	
	socketOpts = fse.socketOpts;
	inTimeout = fse.inTimeout;
	outTimeout = fse.outTimeout;
	inSocketBuffer = fse.inSocketBuffer;
	outSocketBuffer = fse.outSocketBuffer;
	lingerOpts = fse.lingerOpts;
	blocked = fse.blocked;
	
	opened = fse.opened;
	
	fse.opened = false;
	fse.socket = -1;
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange()
{
}

//-------------------------------------------------------------------

flushSocketExchange::flushSocketExchange(__initialAccept &a_init)
{
	init(a_init.socket,a_init.blockInherited);
}

//-------------------------------------------------------------------

flushSocketExchange::~flushSocketExchange()
{
	close();
}

//-------------------------------------------------------------------

void
flushSocketExchange::init(__initialAccept &a_init)
{
	family = a_init.family;
	type = a_init.type;
	
	blocked = a_init.blocked;
	
	init(a_init.socket, a_init.blockInherited);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::close() const
{		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_CLOSE;
		performXExec(preExec);
	#endif
	
	if (!opened)
		#ifndef NO_EX
			return ;
		#else
			return true;
		#endif
	
	#ifdef NO_EX
		register bool result = 
	#endif
	flushSocketOptions::_close(socket);
	
	opened = false;
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
		performXExec(postExec);
	#endif
			
	#ifdef NO_EX
		return result;
	#endif			
}

//-------------------------------------------------------------------

void 
flushSocketExchange::init(int a_socket, 
						bool blockInherited)
{
	if (socket != a_socket)
	{
		close();
	
		socket = a_socket;
	
		setInBufferSize(inSocketBuffer);
		setOutBufferSize(outSocketBuffer);
		
		setInTimeout(inTimeout);
		setOutTimeout(outTimeout);
		
		setLingerSockOption(lingerOpts,lingerSeconds);	
		
		if (blocked)
		{
			if (blockInherited)
				block(true);
			else
				block(false);
		}
	
		opened = true;
	}
}

//-------------------------------------------------------------------

bool 
flushSocketExchange::alive() const
{
	return opened;
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::send(const char * const data, 
						bool urgent) const
{
	buffer.assign(data,outSize);
				
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_SEND;
		performXExec(preExec);
	#endif	
	
	iter = outSize/outSocketBuffer;
	rest = outSize%outSocketBuffer;
	
	register long n(0), sent(0);	
		
	register int flag = 0;	
	if (urgent)	
		flag = MSG_OOB;
		
	for (register long i=0;i<iter;++i)
	{
		n = 0;
		while (sent<outSize)
		{
			n = ::send(socket,buffer.c_str()+sent,inSocketBuffer,flag);
			if (n==-1)
				#ifndef NO_EX
				{
					if (errno == EINVAL || errno == EWOULDBLOCK)
						return ;
					else
						throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_SEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				}
				#else
					return false;	
				#endif
			sent += inSocketBuffer;
		}
	}
	
	if (rest>0)
	{
		n = 0;
		while (sent<rest)
		{
			n = ::send(socket,buffer.c_str()+sent,rest,flag);
			if (n==-1)
				#ifndef NO_EX
				{
					if (errno == EINVAL || errno == EWOULDBLOCK)
						return ;
					else
						throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_SEND,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
				}
				#else
					return false;	
				#endif
			sent += n;
		}		
	}
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
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
flushSocketExchange::sendString(const std::string &data, 
						bool urgent) const
{
	return this->send(data.c_str(),urgent);
}

//-------------------------------------------------------------------

#ifndef NO_EX
	void 
#else
	bool
#endif
flushSocketExchange::receive(char * const data, 
							bool urgent) const
{
		
	#ifndef FLUSH_SOCKET_WO_XEXEC
		operType = FLUSHSOCKETEXCHANGE_OPER_RECIEVE;
		performXExec(preExec);
	#endif	
	
	memset(data,'\0',inSize);
				
	iter = inSize/inSocketBuffer;
	rest = inSize%inSocketBuffer;
	
	register long n(0), received(0);
		
	register int flag = 0;	
	if (urgent)	
		flag = MSG_OOB;
			
	for (register long i=0;i<iter;++i)
	{
		n = 0;
		n = ::recv(socket,data+received,inSocketBuffer,flag);
		if (n==-1)
			#ifndef NO_EX
			{
				if (errno == EINVAL || errno == EWOULDBLOCK)
					return ;
				else
					throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_RECIEVE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			}
			#else
				return false;	
			#endif
		received += n;
	}
	
	if (rest>0)
		if (::recv(socket,data+received,rest,flag)==-1)
			#ifndef NO_EX
			{
				if (errno == EINVAL || errno == EWOULDBLOCK)
					return ;
				else
					throw baseEx(ERRMODULE_FLUSHSOCKETEXCHANGE,FLUSHSOCKETEXCHANGE_RECIEVE,ERR_ERRNO,errno,strerror(errno),__LINE__,__FILE__);
			}
			#else
				return false;	
			#endif
	
	buffer.assign(data,inSize);
			
	#ifndef FLUSH_SOCKET_WO_XEXEC		
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
flushSocketExchange::receiveString(std::string &data, 
								bool urgent) const
{	
	register char *t_data = new char[inSize+1];

	#ifdef NO_EX
		register bool result = 
	#endif
	
	this->receive(t_data,urgent);
	data.assign(t_data,inSize);
	
	delete [] t_data;
	
	#ifdef NO_EX	
		return result;
	#endif
}

//-------------------------------------------------------------------

#ifndef FLUSH_SOCKETEXCHANGE_WO_XEXEC

	int 
	flushSocketExchange::addPostExec(inExec func, 
						void *data) const
	{
		return _addPostExec(func, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data);
	}
	
	//-------------------------------------------------------------------
	
	int 
	flushSocketExchange::addPreExec(inExec func, 
						void *data) const
	{
		return _addPreExec(func, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data);
	}
	
	//-------------------------------------------------------------------
	
	#ifdef DL_EXT
	
		int 
		flushSocketExchange::addPostExec(const std::string &module, 
							void *data) const
		{
			return _addPostExec(module, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data);
		}
		
		//-------------------------------------------------------------------
		
		int 
		flushSocketExchange::addPreExec(const std::string &module, 
							void *data) const
		{
			return _addPreExec(module, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data);
		}
		
		//-------------------------------------------------------------------
		
		xexecCounts 
		flushSocketExchange::addExec(const std::string &module, 
							void *data) const
		{
			return _addExec(module, (void *)this, XEXECOBJ_FLUSHSOCKETEXCHANGE, data);
		}
	
	#endif

#endif

//-------------------------------------------------------------------
