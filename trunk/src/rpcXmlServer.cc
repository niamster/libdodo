/***************************************************************************
 *            rpcXmlServer.cc
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

#include <libdodo/rpcXmlServer.h>

using namespace dodo;

rpcXmlServer::rpcXmlServer()
{
	
}

//-------------------------------------------------------------------

rpcXmlServer::~rpcXmlServer()
{
	
}

//-------------------------------------------------------------------

rpcMethod 
rpcXmlServer::processRPCCall(const dodoString &data)
{
	rpcMethod method;
	
	return method;
}

//-------------------------------------------------------------------

dodoString 
rpcXmlServer::processRPCCallResult(const rpcResponse &response)
{
	dodoString result;
	
	return result;
}

//-------------------------------------------------------------------
