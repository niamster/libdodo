/***************************************************************************
 *            rpcHttpClient.cc
 *
 *  Sat Apr 12 16:49:55 2008
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

#include <libdodo/rpcHttpClient.h>

using namespace dodo::rpc::http;

client::client(const dodoString &ct) : ct(ct)
{

}

//-------------------------------------------------------------------

client::~client()
{

}

//-------------------------------------------------------------------

void
client::sendTextRequest(const dodoString &method)
{
	http.POST(method, ct);
}

//-------------------------------------------------------------------

dodoString
client::receiveTextResponse()
{
	io::network::__httpResponse response = http.getResponse();

	return response.data;
}

//-------------------------------------------------------------------

void
client::setUrl(const dodoString &url)
{
	http.setUrl(url);
}

//-------------------------------------------------------------------