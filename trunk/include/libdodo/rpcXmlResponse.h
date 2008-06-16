/***************************************************************************
 *            rpcXmlResponse.h
 *
 *  Wed Apr 09 23:30:55 2008
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

#ifndef _RPCXMLRESPONSE_H_
#define _RPCXMLRESPONSE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/rpcXmlValue.h>
#include <libdodo/xmlProcessor.h>

namespace dodo
{
	namespace rpc
	{
		namespace xml
		{
			/**
			 * @class response defines RPC response in XML representation
			 */
			class response
			{
				friend class server;

				public:

					/**
					 * @return response parsed from XML
					 * @param data defines XML string
					 */
					static rpc::response xmlToRpcResponse(const dodoString &data);

					/**
					 * @return XML parsed from response
					 * @param data defines response structure
					 */
					static dodoString responseToXml(const rpc::response &data);

				protected:

					static const char trimSymbols[2];                     ///< symbols to trim in the end and in the begining of the XML node value

					/**
					 * @return response parsed from XML node
					 * @param node defines XML node
					 */
					static rpc::response xmlToRpcResponse(dodo::xml::node &node);

					/**
					 * @return XML node parsed from response
					 * @param data defines response structure
					 */
					static dodo::xml::node responseToXmlNode(const rpc::response &data);
			};
		};
	};
};

#endif
