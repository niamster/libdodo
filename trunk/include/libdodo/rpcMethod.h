/***************************************************************************
 *            rpcMethod.h
 *
 *  Sat Mar 22 00:06:55 2008
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

#ifndef _RPCMETHOD_H_
#define _RPCMETHOD_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/rpcValue.h>

namespace dodo
{	
	/**
	 * @class rpcMethod defines RPC method reprasentation
	 */
	class rpcMethod
	{
		friend class rpcClient;
		friend class rpcServer;
		friend class rpcXmlMethod;
		
		public:

			/**
			 * constructor
			 */
			rpcMethod();

			/**
			 * destructor
			 */
			virtual ~rpcMethod();

			/**
			 * set method name
			 * @param name defines method name
			 */
			virtual void setName(const dodoString &name);

			/**
			 * clear arguments information
			 */
			virtual void clear();
			
			/**
			 * add argument
			 * @param argument defines method argument 
			 */
			virtual void addArgument(const rpcValue &argument);
			
		private:
			
			dodoArray<rpcValue> arguments;///< method arguments
			dodoString name;///< method name
	};
};

#endif
