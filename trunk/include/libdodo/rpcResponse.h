/***************************************************************************
 *            rpcResponse.h
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

#ifndef _RPCRESPONSE_H_
#define _RPCRESPONSE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/stringTools.h>
#include <libdodo/rpcValue.h>

namespace dodo
{	
	/**
	 * @class rpcResponse defines RPC response reprasentation
	 */
	class rpcResponse
	{
		friend class rpcClient;
		friend class rpcServer;
		
		public:

			/**
			 * constructor
			 */
			rpcResponse();

			/**
			 * destructor
			 */
			virtual ~rpcResponse();

			/**
			 * @return response value
			 */
			virtual rpcValue getValue();
			
			/**
			 * @return true if request has  
			 */
			virtual bool successful();
			
			/**
			 * @return rpcValue for type casting  
			 */
			virtual operator rpcValue(); 
			
		private:
			
			rpcValue value;///< response value
			bool succ;///< true if server returned non-fail response
	};
};

#endif
