/***************************************************************************
 *            rpcXmlValue.h
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

#ifndef _RPCXMLVALUE_H_
#define _RPCXMLVALUE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcValue.h>
#include <libdodo/xml.h>

namespace dodo
{
	/**
	 * @class rpcXmlValue defines RPC value in XML representation
	 */
	class rpcXmlValue
	{
		friend class rpcXmlMethod;
		friend class rpcXmlResponse;
		
		public:
			
			/**
			 * @return rpcValue parsed from XML
			 * @param data defines XML string
			 */
			static rpcValue xmlToRpcValue(const dodoString &data);
			
			/**
			 * @return XML parsed from rpcValue
			 * @param data defines rpcValue structure
			 */
			static dodoString rpcValueToXml(const rpcValue &data); 
		
		protected:
			
			static const char trimSymbols[2];///< symbols to trim in the end and in the begining of the XML node value  
			
			/**
			 * @return rpcValue parsed from XML node
			 * @param data defines XML node
			 */
			static rpcValue xmlToRpcValue(__xmlNode &node);
			
			/**
			 * @return XML node from rpcValue
			 * @param data defines rpcValue structure
			 */
			static __xmlNode rpcValueToXmlNode(const rpcValue &data); 
	};
};

#endif
