/***************************************************************************
 *            rpcCgiServer.h
 *
 *  Sat Apr 12 23:06:55 2008
 *  Copyright  2008  Ni@m
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

#ifndef _RPCCGISERVER_H_
#define _RPCCGISERVER_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/cgiServer.h>
#include <libdodo/rpcServer.h>

namespace dodo
{
	namespace rpc
	{
		namespace cgi
		{
			/**
			 * @class server defines server-side RPC instrument
			 */
			class server : virtual public rpc::server
			{
				public:

					/**
					 * constructor
					 * @param provider defines cgi I/O provider
					 * @param ct defines content-type of the response
					 */
					server(dodo::cgi::server &provider, const dodoString &ct);

					/**
					 * destructor
					 */
					virtual ~server();

				protected:

					/**
					 * send request
					 * @param response defines rpc method call
					 */
					virtual void sendTextRequest(const dodoString &response);

					/**
					 * get response
					 * @return rpc response result
					 */
					virtual dodoString receiveTextResponse();

					dodo::cgi::server &provider;
			};
		};
	};
};

#endif
