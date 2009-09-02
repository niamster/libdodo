/***************************************************************************
 *            ioNetworkConnection.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioNetworkConnectionEx.h>

using namespace dodo::io::network;

connection::connection() : lingerOpts(IONETWORKCONNECTION_SOCKET_LINGER_OPTION),
						   lingerSeconds(IONETWORKCONNECTION_SOCKET_LINGER_PERIOD),
						   inSocketTimeout(IONETWORKCONNECTION_SOCKET_RECEIVE_TIMEOUT),
						   outSocketTimeout(IONETWORKCONNECTION_SOCKET_SEND_TIMEOUT),
						   inSocketBufferSize(IONETWORKCONNECTION_SOCKETBUFFER_INSIZE),
						   outSocketBufferSize(IONETWORKCONNECTION_SOCKETBUFFER_OUTSIZE),
						   socket(-1),
						   blocked(true)
{
}

//-------------------------------------------------------------------

connection::~connection()
{
}

//-------------------------------------------------------------------

bool
connection::isBlocked() const
{
	return blocked;
}

//-------------------------------------------------------------------

void
connection::block(bool flag)
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_BLOCK, exception::ERRNO_LIBDODO, CONNECTIONEX_NOSOCKETCREATED, IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	int block = fcntl(socket, F_GETFL);
	if (block == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (flag)
		block &= ~O_NONBLOCK;
	else
		block |= O_NONBLOCK;

	if (fcntl(socket, F_SETFL, block) == 1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_BLOCK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	blocked = flag;
}

//-------------------------------------------------------------------

void
connection::setInBufferSize(unsigned long bytes)
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETINBUFFERSIZE, exception::ERRNO_LIBDODO, CONNECTIONEX_NOSOCKETCREATED, IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	inSocketBufferSize = bytes;

	if (setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &inSocketBufferSize, sizeof(long)) == 1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETINBUFFERSIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
connection::inBufferSize() const
{
	return inSocketBufferSize;
}

//-------------------------------------------------------------------

void
connection::setOutBufferSize(unsigned long bytes)
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETOUTBUFFERSIZE, exception::ERRNO_LIBDODO, CONNECTIONEX_NOSOCKETCREATED, IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	outSocketBufferSize = bytes;

	if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &outSocketBufferSize, sizeof(long)) == 1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETOUTBUFFERSIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
connection::outBufferSize() const
{
	return outSocketBufferSize;
}

//-------------------------------------------------------------------

void
connection::setInTimeout(unsigned long microseconds)
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETINTIMEOUT, exception::ERRNO_LIBDODO, CONNECTIONEX_NOSOCKETCREATED, IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	inSocketTimeout = microseconds;

	timeval val;
	val.tv_sec = inSocketTimeout / 100;
	val.tv_usec = inSocketTimeout % 100;

	if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &val, sizeof(val)) == 1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETINTIMEOUT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
connection::inTimeout() const
{
	return inSocketTimeout;
}

//-------------------------------------------------------------------

void
connection::setOutTimeout(unsigned long microseconds)
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETOUTTIMEOUT, exception::ERRNO_LIBDODO, CONNECTIONEX_NOSOCKETCREATED, IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	outSocketTimeout = microseconds;

	timeval val;
	val.tv_sec = outSocketTimeout / 100;
	val.tv_usec = outSocketTimeout % 100;

	if (setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, &val, sizeof(val)) == 1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETOUTTIMEOUT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

unsigned long
connection::outTimeout() const
{
	return outSocketTimeout;
}

//-------------------------------------------------------------------

bool
connection::isSetOption(int option) const
{
	if  ((option & socketOpts) == option)
		return true;

	return false;
}

//-------------------------------------------------------------------

void
connection::setOption(short option,
					  bool  flag)
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETSOCKOPT, exception::ERRNO_LIBDODO, CONNECTIONEX_NOSOCKETCREATED, IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	int sockFlag(1);

	if (!flag)
		sockFlag = 0;

	int real_option(0);

	switch (option) {
		case OPTION_KEEP_ALIVE:

			real_option = SO_KEEPALIVE;

			break;

		case OPTION_REUSE_ADDRESS:

			real_option = SO_REUSEADDR;

			break;

		case OPTION_DONOT_USE_GATEWAY:

			real_option = SO_DONTROUTE;

			break;

		case OPTION_BROADCAST:

			real_option = SO_BROADCAST;

			break;

		case OPTION_OOB_INLINE:

			real_option = SO_OOBINLINE;

			break;

		default:

			throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETSOCKOPT, exception::ERRNO_LIBDODO, CONNECTIONEX_WRONGPARAMETER, IONETWORKCONNECTIONEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (setsockopt(socket, SOL_SOCKET, real_option, &sockFlag, sizeof(int)) == 1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETSOCKOPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (!flag)
		removeFlag(socketOpts, 1 << option);
	else
		addFlag(socketOpts, 1 << option);
}

//-------------------------------------------------------------------

void
connection::setLingerOption(short option,
							int   seconds)
{
	if (socket == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETLINGERSOCKOPT, exception::ERRNO_LIBDODO, CONNECTIONEX_NOSOCKETCREATED, IONETWORKCONNECTIONEX_NOSOCKETCREATED_STR, __LINE__, __FILE__);

	linger lin;

	switch (option) {
		case LINGER_OPTION_GRACEFUL_CLOSE:

			lin.l_onoff = 0;

			break;

		case LINGER_OPTION_HARD_CLOSE:

			lin.l_onoff = 1;
			lin.l_linger = 0;

			break;

		case LINGER_OPTION_WAIT_CLOSE:

			lin.l_onoff = 1;
			lin.l_linger = seconds;

			break;

		default:

			throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETLINGERSOCKOPT, exception::ERRNO_LIBDODO, CONNECTIONEX_WRONGPARAMETER, IONETWORKCONNECTIONEX_WRONGPARAMETER_STR, __LINE__, __FILE__);
	}

	if (setsockopt(socket, SOL_SOCKET, SO_LINGER, &lin, sizeof(linger)) == 1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX_SETLINGERSOCKOPT, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	lingerOpts = option;
	lingerSeconds = seconds;
}

//-------------------------------------------------------------------

short
connection::lingerOption() const
{
	return lingerOpts;
}

//-------------------------------------------------------------------

int
connection::lingerPeriod() const
{
	return lingerSeconds;
}

//-------------------------------------------------------------------

void
connection::_close(int socket)
{
	if (::shutdown(socket, SHUT_RDWR) == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX__CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::close(socket) == -1)
		throw exception::basic(exception::MODULE_IONETWORKCONNECTION, CONNECTIONEX__CLOSE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

