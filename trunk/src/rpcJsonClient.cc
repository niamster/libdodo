/***************************************************************************
 *            rpcJsonClient.cc
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

#include <libdodo/directives.h>

#include <libdodo/rpcJsonClient.h>
#include <libdodo/types.h>
#include <libdodo/rpcClient.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/rpcJsonMethod.h>
#include <libdodo/dataFormatJsonNode.h>
#include <libdodo/dataFormatJsonProcessor.h>
#include <libdodo/rpcJsonResponse.h>
#include <libdodo/ioMemory.h>

using namespace dodo::rpc::json;

client::client(io::channel &io) : rpc::client(io),
								  rqVersion("1.1"),
								  rqId(0)
{
}

//-------------------------------------------------------------------

client::~client()
{
}

//-------------------------------------------------------------------

void
client::processCallRequest(const rpc::method &meth)
{
	dodo::data::format::json::processor jsonValue;

	jsonValue.make(method::methodToJson(meth, rqVersion, ++rqId), io);
}

//-------------------------------------------------------------------

dodo::rpc::response
client::processCallResult()
{
	dodo::data::format::json::processor jsonValue;

	dodo::data::format::json::node node = jsonValue.process(io);

	return response::jsonToResponse(node, rpVersion, rpId);
}

//-------------------------------------------------------------------

void
client::setRequestVersion(const dodoString &version)
{
	rqVersion = version;
}

//-------------------------------------------------------------------

dodoString
client::getResponseVersion()
{
	return rpVersion;
}

//-------------------------------------------------------------------

long
client::getResponseId()
{
	return rpId;
}

//-------------------------------------------------------------------

