/***************************************************************************
 *            rpcXmlCgiServer.h
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

#ifndef _RPCXMLCGISERVER_H_
#define _RPCXMLCGISERVER_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/cgi.h>
#include <libdodo/rpcXmlServer.h>

namespace dodo
{	
	/**
	 * @class rpcServer defines server-side RPC instrument
	 */
	class rpcXmlCgiServer : public rpcXmlServer
	{
		public:

			/**
			 * constructor
			 */
			rpcXmlCgiServer(cgi &provider);

			/**
			 * destructor
			 */
			virtual ~rpcXmlCgiServer();
		
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
			
			cgi &provider;
	};
};

#endif
