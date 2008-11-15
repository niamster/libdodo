/***************************************************************************
 *            rpcJsonCgiServer.h
 *
 *  Mon Jul 07 11:29:55 2008
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

#ifndef _RPCJSONCGISERVER_H_
#define _RPCJSONCGISERVER_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/cgiDialogue.h>
#include <libdodo/rpcCgiServer.h>
#include <libdodo/rpcJsonServer.h>

namespace dodo
{
	namespace rpc
	{
		namespace json
		{
			namespace cgi
			{
				/**
				 * @class server
				 * @brief defines server-side RPC instrument
				 */
				class server : public json::server,
							   public rpc::cgi::server
				{
					public:

						/**
						 * constructor
						 * @param provider defines cgi I/O provider
						 */
						server(dodo::cgi::dialogue &provider);

						/**
						 * destructor
						 */
						virtual ~server();
				};
			};
		};
	};
};

#endif
