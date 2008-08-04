/***************************************************************************
 *            rpcXmlClient.cc
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/rpcXmlClient.h>

using namespace dodo::rpc::xml;

client::client() : rqEncoding("UTF-8")
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
client::getResponseEncoding()
{
	return rpEncoding;
}

//-------------------------------------------------------------------

dodoString
client::processCall(const rpc::method &meth)
{
	dodo::data::format::xml::processor xmlValue;

	return xmlValue.make(method::methodToXml(meth), rqEncoding);
}

//-------------------------------------------------------------------

dodo::rpc::response
client::processCallResult(const dodoString &data)
{
	dodo::data::format::xml::processor xmlValue;
	
	rpEncoding = xmlValue.getBufferInfo(data).encoding;

	dodo::data::format::xml::__nodeDef xmlMethodResponse;
	xmlMethodResponse.name = "methodResponse";
	xmlMethodResponse.ignoreChildrenDef = true;

	dodo::data::format::xml::node node = xmlValue.processBuffer(xmlMethodResponse, data);

	return response::xmlToResponse(node);
}

//-------------------------------------------------------------------
