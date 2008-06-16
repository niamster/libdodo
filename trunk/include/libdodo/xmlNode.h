/***************************************************************************
 *            xmlNode.h
 *
 *  Tue Nov 29 23:31:55 2005
 *  Copyright  2005  Ni@m
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

#ifndef _XMLNODE_H_
#define _XMLNODE_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo
{
	namespace xml
	{
		/**
		 * @class node defines processor tree
		 * @note children[x][y]: [x] defines vector of processor nodes[y] that are realisation of __nodeDef::children[x] definition
		 */
		class node
		{
			public:

				/**
				 * constructor
				 */
				node();

				dodoString name;                                                                                ///< name of the node [[tag]]

				dodoMap<dodoString, dodoArray<node>, dodoMapStringCompare> children;                            ///< children

				dodoStringMap attributes;                                                                       ///< attributes

				dodoString value;                                                                               ///< value of the node

				dodoString ns;                                                                                  ///< namespace of the node
				dodoString nsHref;                                                                              ///< uri "value"
				dodoString nsDef;                                                                               ///< namespace
				dodoString nsDefHref;                                                                           ///< namespace URI

				bool CDATA;                                                                                     ///< true if CDATA
				bool empty;                                                                                     ///< true if node doesn't have contents : `<node />`
		};
	};
};

#endif

