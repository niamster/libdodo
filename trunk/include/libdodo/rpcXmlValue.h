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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _RPCXMLVALUE_H_
#define _RPCXMLVALUE_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo
{
	namespace data
	{
		namespace format
		{
			namespace xml
			{
				class node;
			};
		};
	};

	namespace rpc
	{
		class value;

		namespace xml
		{
			/**
			 * @class value
			 * @brief defines RPC value in XML representation
			 */
			class value
			{
			  public:

				/**
				 * @return value parsed from XML node
				 * @param node defines XML node
				 */
				static rpc::value xmlToValue(dodo::data::format::xml::node &node);

				/**
				 * @return XML node from value
				 * @param data defines value structure
				 */
				static dodo::data::format::xml::node valueToXml(const rpc::value &data);

			  protected:

				static const char trimSymbols[2]; ///< symbols to trim in the end and in the begining of the XML node value
			};
		};
	};
};

#endif
