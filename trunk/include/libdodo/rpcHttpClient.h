/***************************************************************************
 *            rpcHttpClient.h
 *
 *  Sat Apr 12 16:49:55 2008
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

#ifndef _RPCHTTPCLIENT_H_
#define _RPCHTTPCLIENT_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/ioNetworkHttp.h>
#include <libdodo/rpcClient.h>

namespace dodo
{
	namespace rpc
	{
		namespace http
		{
			/**
			 * @class client defines client-side RPC instrument
			 */
			class client : virtual public rpc::client
			{
				public:

					/**
					 * constructor
					 * @param ct defines content type of the request
					 */
					client(const dodoString &ct);

					/**
					 * destructor
					 */
					virtual ~client();

					/**
					 * set url to RPC server
					 * @param url define url to RPC server
					 */
					virtual void setUrl(const dodoString &url);

				protected:

					/**
					 * send request
					 * @param method defines rpc method call
					 */
					virtual void sendTextRequest(const dodoString &method);

					/**
					 * get response
					 * @return rpc response result
					 */
					virtual dodoString receiveTextResponse();

					io::network::http http;///< http handler

					dodoString ct;///< content-type of the request

			};
		};
	};
};

#endif
