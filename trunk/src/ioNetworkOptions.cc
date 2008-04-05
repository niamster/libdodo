/***************************************************************************
 *            ioNetworkOptions.cc
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

#include <libdodo/ioNetworkOptions.h>

using namespace dodo;

ioNetworkOptions::ioNetworkOptions(short a_family,
								 short a_type) : family(a_family),
												 type(a_type),
												 lingerOpts(IONETWORKOPTIONS_SOCKET_LINGER_OPTION),
												 lingerSeconds(IONETWORKOPTIONS_SOCKET_LINGER_PERIOD),
												 inTimeout(IONETWORK_RECIEVE_TIMEOUT),
												 outTimeout(IONETWORK_SEND_TIMEOUT),
												 inSocketBuffer(IONETWORKOPTIONS_SOCKET_INSIZE),
												 outSocketBuffer(IONETWORKOPTIONS_SOCKET_OUTSIZE),
												 socket(-1),
												 blocked(true)
{
}

//-------------------------------------------------------------------

ioNetworkOptions::ioNetworkOptions() : lingerOpts(IONETWORKOPTIONS_SOCKET_LINGER_OPTION),
									 lingerSeconds(IONETWORKOPTIONS_SOCKET_LINGER_PERIOD),
									 inTimeout(IONETWORK_RECIEVE_TIMEOUT),
									 outTimeout(IONETWORK_SEND_TIMEOUT),
									 inSocketBuffer(IONETWORKOPTIONS_SOCKET_INSIZE),
									 outSocketBuffer(IONETWORKOPTIONS_SOCKET_OUTSIZE),
									 socket(-1),
									 blocked(true)
{
}

//-------------------------------------------------------------------

ioNetworkOptions::~ioNetworkOptions()
{
}

//-------------------------------------------------------------------

bool
ioNetworkOptions::isBlocked() const
{
	return blocked;
}

//-------------------------------------------------------------------

void
ioNetworkOptions::block(bool flag)
{
	int block = fcntl(socket, F_GETFL);
	if (block == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (flag)
		block &= ~O_NONBLOCK;
	else
		block |= O_NONBLOCK;

	if (fcntl(socket, F_SETFL, block) == 1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_BLOCK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

void
ioNetworkOptions::setInBufferSize(unsigned long bytes)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETINBUFFERSIZE, ERR_LIBDODO, IONETWORKOPTIONSEX_NOSOCKETCREATED, IONETWORKOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	inSocketBuffer = bytes;

	if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &inSocketBuffer, sizeof(long)) == 1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETINBUFFERSIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
ioNetworkOptions::getInBufferSize() const
{
	return inSocketBuffer;
}

//-------------------------------------------------------------------

void
ioNetworkOptions::setOutBufferSize(unsigned long bytes)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETOUTBUFFERSIZE, ERR_LIBDODO, IONETWORKOPTIONSEX_NOSOCKETCREATED, IONETWORKOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	outSocketBuffer = bytes;

	if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &outSocketBuffer, sizeof(long)) == 1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETOUTBUFFERSIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
ioNetworkOptions::getOutBufferSize() const
{
	return outSocketBuffer;
}

//-------------------------------------------------------------------

void
ioNetworkOptions::setInTimeout(unsigned long microseconds)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETINTIMEOUT, ERR_LIBDODO, IONETWORKOPTIONSEX_NOSOCKETCREATED, IONETWORKOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	inTimeout = microseconds;

	timeval val;
	val.tv_sec = inTimeout / 100;
	val.tv_usec = inTimeout % 100;

	if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof(val)) == 1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETINTIMEOUT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
ioNetworkOptions::getInTimeout() const
{
	return inTimeout;
}

//-------------------------------------------------------------------

void
ioNetworkOptions::setOutTimeout(unsigned long microseconds)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETOUTTIMEOUT, ERR_LIBDODO, IONETWORKOPTIONSEX_NOSOCKETCREATED, IONETWORKOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	outTimeout = microseconds;

	timeval val;
	val.tv_sec = outTimeout / 100;
	val.tv_usec = outTimeout % 100;

	if (setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof(val)) == 1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETOUTTIMEOUT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}


//-------------------------------------------------------------------

unsigned long
ioNetworkOptions::getOutTimeout() const
{
	return outTimeout;
}

//-------------------------------------------------------------------

bool
ioNetworkOptions::getOption(int option) const
{
	if  ((option & socketOpts) == option)
		return true;
	return false;
}

//-------------------------------------------------------------------

void
ioNetworkOptions::setOption(short option,
							   bool flag)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETSOCKOPT, ERR_LIBDODO, IONETWORKOPTIONSEX_NOSOCKETCREATED, IONETWORKOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	int sockFlag(1);

	if (!flag)
		sockFlag = 0;

	int real_option(0);

	switch (option)
	{
		case IONETWORKOPTIONS_OPTION_KEEP_ALIVE:

			real_option = SO_KEEPALIVE;

			break;

		case IONETWORKOPTIONS_OPTION_REUSE_ADDRESS:

			real_option = SO_REUSEADDR;

			break;

		case IONETWORKOPTIONS_OPTION_DONOT_USE_GATEWAY:

			real_option = SO_DONTROUTE;

			break;

		case IONETWORKOPTIONS_OPTION_BROADCAST:

			real_option = SO_BROADCAST;

			break;

		case IONETWORKOPTIONS_OPTION_OOB_INLINE:

			real_option = SO_OOBINLINE;

			break;

#ifdef SO_REUSEPORT

		case IONETWORKOPTIONS_OPTION_REUSE_PORT:

			real_option = SO_REUSEPORT;

			break;

#endif

		default:

			throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETSOCKOPT, ERR_LIBDODO, IONETWORKOPTIONSEX_WRONGPARAMETER, IONETWORKOPTIONSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (setsockopt(socket, SOL_SOCKET, real_option, &sockFlag, sizeof(int)) == 1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETSOCKOPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (!flag)
		removeFlag(socketOpts, 1 << option);
	else
		addFlag(socketOpts, 1 << option);
}

//-------------------------------------------------------------------

void
ioNetworkOptions::setLingerOption(short option,
									 int seconds)
{
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETLINGERSOCKOPT, ERR_LIBDODO, IONETWORKOPTIONSEX_NOSOCKETCREATED, IONETWORKOPTIONSEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	linger lin;

	switch (option)
	{
		case IONETWORKOPTIONS_LINGEROPTION_GRACEFUL_CLOSE:

			lin.l_onoff = 0;

			break;

		case IONETWORKOPTIONS_LINGEROPTION_HARD_CLOSE:

			lin.l_onoff = 1;
			lin.l_linger = 0;

			break;

		case IONETWORKOPTIONS_LINGEROPTION_WAIT_CLOSE:

			lin.l_onoff = 1;
			lin.l_linger = seconds;

			break;

		default:

			throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETLINGERSOCKOPT, ERR_LIBDODO, IONETWORKOPTIONSEX_WRONGPARAMETER, IONETWORKOPTIONSEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (setsockopt(socket, SOL_SOCKET, SO_LINGER, &lin, sizeof(linger)) == 1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX_SETLINGERSOCKOPT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	lingerOpts = option;
	lingerSeconds = seconds;
}

//-------------------------------------------------------------------

short
ioNetworkOptions::getLingerOption() const
{
	return lingerOpts;
}

//-------------------------------------------------------------------

int
ioNetworkOptions::getLingerPeriod() const
{
	return lingerSeconds;
}

//-------------------------------------------------------------------

void
ioNetworkOptions::_close(int socket)
{
	if (::shutdown(socket, SHUT_RDWR) == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX__CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::close(socket) == -1)
		throw baseEx(ERRMODULE_IONETWORKOPTIONS, IONETWORKOPTIONSEX__CLOSE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

