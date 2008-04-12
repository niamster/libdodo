/***************************************************************************
 *            rpcXmlMethod.h
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

#ifndef _RPCXMLMETHOD_H_
#define _RPCXMLMETHOD_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/stringTools.h>
#include <libdodo/rpcMethod.h>
#include <libdodo/rpcXmlValue.h>
#include <libdodo/xml.h>

namespace dodo
{	
	/**
	 * @class rpcXmlMethod defines RPC method in XML representation
	 */
	class rpcXmlMethod
	{	
		public:
			
			/**
			 * @return rpcMethod parsed from XML
			 * @param data defines XML string
			 */
			static rpcMethod xmlToRpcMethod(const dodoString &data);
			
			/**
			 * @return XML parsed from rpcMethod
			 * @param data defines rpcMethod structure
			 */
			static dodoString rpcMethodToXml(const rpcMethod &data); 
			
		protected:
			
			static const char trimSymbols[2];///< symbols to trim in the end and in the begining of the XML node value
			
			/**
			 * @return rpcMethod parsed from XML node
			 * @param data defines XML string
			 */
			static rpcMethod xmlToRpcMethod(__xmlNode &node);
			
			/**
			 * @return XML node parsed from rpcMethod
			 * @param data defines rpcMethod structure
			 */
			static __xmlNode rpcMethodToXmlNode(const rpcMethod &data); 
	};
};

#endif
