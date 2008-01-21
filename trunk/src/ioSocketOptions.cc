/***************************************************************************
 *            ioSocketOptions.cc
 *
 *  Thu Sep 20 01:43:24 2005
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

#include <libdodo/ioSocketOptions.h>

using namespace dodo;

ioSocketOptions::ioSocketOptions(short a_family,
								 short a_type) : family(a_family),
												 type(a_type),
												 lingerOpts(IOSOCKETOPTIONS_SOCKET_LINGER_OPTION),
												 lingerSeconds(IOSOCKETOPTIONS_SOCKET_LINGER_PERIOD),
												 inTimeout(IOSOCKET_RECIEVE_TIMEOUT),
												 outTimeout(IOSOCKET_SEND_TIMEOUT),
												 inSocketBuffer(IOSOCKETOPTIONS_SOCKET_INSIZE),
												 outSocketBuffer(IOSOCKETOPTIONS_SOCKET_OUTSIZE),
												 socket(-1),
												 blocked(true)
{
}

//-------------------------------------------------------------------

ioSocketOptions::ioSocketOptions() : lingerOpts(IOSOCKETOPTIONS_SOCKET_LINGER_OPTION),
									 lingerSeconds(IOSOCKETOPTIONS_SOCKET_LINGER_PERIOD),
									 inTimeout(IOSOCKET_RECIEVE_TIMEOUT),
									 outTimeout(IOSOCKET_SEND_TIMEOUT),
									 inSocketBuffer(IOSOCKETOPTIONS_SOCKET_INSIZE),
									 outSocketBuffer(IOSOCKETOPTIONS_SOCKET_OUTSIZE),
									 socket(-1),
									 blocked(true)
{
}

//-------------------------------------------------------------------

ioSocketOptions::~ioSocketOptions()
{
}

//-------------------------------------------------------------------

bool
ioSocketOptions::isBlocked() const
{
	return blocked;
}

//-------------------------------------------------------------------

void
ioSocketOptions::block(bool flag)
{
	int block = fcntl(socket, F_GETFL);
	if (block == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (flag)
		block &= ~O_NONBLOCK;
	else
		block |= O_NONBLOCK;

	if (fcntl(socket, F_SETFL, block) == 1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

void
ioSocketOptions::setInBufferSize(unsigned long bytes)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETINBUFFERSIZE, ERR_LIBDODO, IOSOCKETOPTIONSEX_NOSOCKETCREATED, IOSOCKETOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	inSocketBuffer = bytes;

	if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &inSocketBuffer, sizeof(long)) == 1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETINBUFFERSIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
ioSocketOptions::getInBufferSize() const
{
	return inSocketBuffer;
}

//-------------------------------------------------------------------

void
ioSocketOptions::setOutBufferSize(unsigned long bytes)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETOUTBUFFERSIZE, ERR_LIBDODO, IOSOCKETOPTIONSEX_NOSOCKETCREATED, IOSOCKETOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	outSocketBuffer = bytes;

	if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &outSocketBuffer, sizeof(long)) == 1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETOUTBUFFERSIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
ioSocketOptions::getOutBufferSize() const
{
	return outSocketBuffer;
}

//-------------------------------------------------------------------

void
ioSocketOptions::setInTimeout(unsigned long microseconds)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETINTIMEOUT, ERR_LIBDODO, IOSOCKETOPTIONSEX_NOSOCKETCREATED, IOSOCKETOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	inTimeout = microseconds;

	timeval val;
	val.tv_sec = inTimeout / 100;
	val.tv_usec = inTimeout % 100;

	if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof(val)) == 1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETINTIMEOUT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
ioSocketOptions::getInTimeout() const
{
	return inTimeout;
}

//-------------------------------------------------------------------

void
ioSocketOptions::setOutTimeout(unsigned long microseconds)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETOUTTIMEOUT, ERR_LIBDODO, IOSOCKETOPTIONSEX_NOSOCKETCREATED, IOSOCKETOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	outTimeout = microseconds;

	timeval val;
	val.tv_sec = outTimeout / 100;
	val.tv_usec = outTimeout % 100;

	if (setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof(val)) == 1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETOUTTIMEOUT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}


//-------------------------------------------------------------------

unsigned long
ioSocketOptions::getOutTimeout() const
{
	return outTimeout;
}

//-------------------------------------------------------------------

bool
ioSocketOptions::getSocketOpts(int option) const
{
	if  ((option & socketOpts) == option)
		return true;
	return false;
}

//-------------------------------------------------------------------

void
ioSocketOptions::setSockOption(short option,
							   bool flag)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETSOCKOPT, ERR_LIBDODO, IOSOCKETOPTIONSEX_NOSOCKETCREATED, IOSOCKETOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	int sockFlag(1);

	if (!flag)
		sockFlag = 0;

	int real_option(0);

	switch (option)
	{
		case IOSOCKETOPTIONS_SOCKET_KEEP_ALIVE:

			real_option = SO_KEEPALIVE;

			break;

		case IOSOCKETOPTIONS_SOCKET_REUSE_ADDRESS:

			real_option = SO_REUSEADDR;

			break;

		case IOSOCKETOPTIONS_SOCKET_DONOT_USE_GATEWAY:

			real_option = SO_DONTROUTE;

			break;

		case IOSOCKETOPTIONS_SOCKET_BROADCAST:

			real_option = SO_BROADCAST;

			break;

		case IOSOCKETOPTIONS_SOCKET_OOB_INLINE:

			real_option = SO_OOBINLINE;

			break;

#ifdef SO_REUSEPORT

		case IOSOCKETOPTIONS_SOCKET_REUSE_PORT:

			real_option = SO_REUSEPORT;

			break;

#endif

		default:

			throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETSOCKOPT, ERR_LIBDODO, IOSOCKETOPTIONSEX_WRONGPARAMETER, IOSOCKETOPTIONSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (setsockopt(socket, SOL_SOCKET, real_option, &sockFlag, sizeof(int)) == 1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETSOCKOPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (!flag)
		removeFlag(socketOpts, 1 << option);
	else
		addFlag(socketOpts, 1 << option);
}

//-------------------------------------------------------------------

void
ioSocketOptions::setLingerSockOption(short option,
									 int seconds)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETLINGERSOCKOPT, ERR_LIBDODO, IOSOCKETOPTIONSEX_NOSOCKETCREATED, IOSOCKETOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	linger lin;

	switch (option)
	{
		case IOSOCKETOPTIONS_SOCKET_GRACEFUL_CLOSE:

			lin.l_onoff = 0;

			break;

		case IOSOCKETOPTIONS_SOCKET_HARD_CLOSE:

			lin.l_onoff = 1;
			lin.l_linger = 0;

			break;

		case IOSOCKETOPTIONS_SOCKET_WAIT_CLOSE:

			lin.l_onoff = 1;
			lin.l_linger = seconds;

			break;

		default:

			throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETLINGERSOCKOPT, ERR_LIBDODO, IOSOCKETOPTIONSEX_WRONGPARAMETER, IOSOCKETOPTIONSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (setsockopt(socket, SOL_SOCKET, SO_LINGER, &lin, sizeof(linger)) == 1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX_SETLINGERSOCKOPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	lingerOpts = option;
	lingerSeconds = seconds;
}

//-------------------------------------------------------------------

short
ioSocketOptions::getLingerOption() const
{
	return lingerOpts;
}

//-------------------------------------------------------------------

int
ioSocketOptions::getLingerPeriod() const
{
	return lingerSeconds;
}

//-------------------------------------------------------------------

void
ioSocketOptions::_close(int socket)
{
	if (::shutdown(socket, SHUT_RDWR) == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX__CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::close(socket) == -1)
		throw baseEx(ERRMODULE_IOSOCKETOPTIONS, IOSOCKETOPTIONSEX__CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

