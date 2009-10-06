/***************************************************************************
 *            rpcXmlClient.cc
 *
 *  Wed Apr 09 2008
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

#include <libdodo/rpcXmlClient.h>
#include <libdodo/types.h>
#include <libdodo/dataFormatXmlNode.h>
#include <libdodo/dataFormatXmlProcessor.h>
#include <libdodo/rpcClient.h>
#include <libdodo/rpcXmlMethod.h>
#include <libdodo/rpcXmlResponse.h>
#include <libdodo/rpcResponse.h>
#include <libdodo/ioChannel.h>

using namespace dodo::rpc::xml;

client::client(const io::channel &io) : rpc::client(io),
										rqEncoding("UTF-8")
{
}

//-------------------------------------------------------------------

client::~client()
{
}

//-------------------------------------------------------------------

void
client::setRequestEncoding(const dodoString &a_encoding)
{
	rqEncoding = a_encoding;
}

//-------------------------------------------------------------------

dodoString
client::responseEncoding()
{
	return rpEncoding;
}

//-------------------------------------------------------------------

void
client::processCallRequest(const rpc::method &meth)
{
	dodo::data::format::xml::processor xmlValue;

	xmlValue.make(method::methodToXml(meth), rqEncoding, "1.0", io);

	io.flush();
}

//-------------------------------------------------------------------

dodo::rpc::response
client::processCallResult()
{
	dodo::data::format::xml::processor xmlValue;

	dodo::data::format::xml::__definition__ xmlMethodResponse;
	xmlMethodResponse.name = "methodResponse";
	xmlMethodResponse.allChildren = true;

	dodo::data::format::xml::node node = xmlValue.process(xmlMethodResponse, io);

	rpEncoding = xmlValue.information().encoding;

	return response::xmlToResponse(node);
}

//-------------------------------------------------------------------
