/***************************************************************************
 *            dataFormatXmlNode.h
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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#ifndef _DATAFORMATXMLNODE_H_
#define _DATAFORMATXMLNODE_H_

#include <libdodo/directives.h>

#include <algorithm>

#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>

namespace dodo
{
	namespace rpc
	{
		namespace xml
		{
			class value;
			class method;
			class response;
		};
	};

	namespace data
	{
		namespace format
		{
			namespace xml
			{
				class processor;

				/**
				 * @struct __nodeNamespace
				 * @brief defines node namespace
				 */
				struct __nodeNamespace
				{
					dodoString prefix; ///< prefix for the namespace
					dodoString href; ///< URL for the namespace
				};

				/**
				 * @class node
				 * @brief defines xml tree
				 * @note children is a map of vectors of xml nodes where map key is node name and vector contains nodes with name given in key
				 */
				class node
				{
					friend class processor;
					friend class rpc::xml::value;
					friend class rpc::xml::method;
					friend class rpc::xml::response;

					public:

						/**
						 * constructor
						 */
						node();

						/**
						 * constructor
						 * @param name defines name of the node
						 * @param attributes defines node attributes
						 * @param value devines node value
						 * @param ns defines node namespace
						 */
						node(const dodoString &name, const dodoStringMap &attributes, const dodoString &value, const dodoString &ns=__dodostring__);

						/**
						 * add child to the node
						 * @param child defines child to be appended
						 */
						virtual void addChild(const node &child);

						/**
						 * set children of the node
						 * @param children defines children to be set
						 */
						virtual void setChildren(const dodoArray<node> &children);

						/**
						 * @return children of the node
						 * @param name defines name of child nodes to get
						 * @param recursive defines if walk through all children in deep
						 */
						virtual dodoArray<node> getChildren(const dodoString &name, bool recursive=false);

						/**
						 * @return names of children nodes
						 * @param recursive defines if walk through all children in deep
						 */
						virtual dodoStringArray getChildrenNames(bool recursive=false);

						/**
						 * @return attribute value
						 * @param name defines attribute name
						 */
						virtual dodoString operator[](const dodoString &name);

						/**
						 * set value of the node
						 * @param value defines value to be set
						 * @param CDATA defines if value is CDATA
						 */
						virtual void setValue(const dodoString &value, bool CDATA=false);

						/**
						 * @return value of the node
						 */
						virtual dodoString getValue();

						dodoStringMap attributes;                                                                                               ///< attributes

						dodoString name;                                                                                                        ///< name of the node [[tag]]

						__nodeNamespace ns;///< namespace of the node
						__nodeNamespace nsDef;///< namespace definition of the node

					protected:

						dodoMap<dodoString, dodoArray<node>, dodoMapStringCompare> children;                                                    ///< children

						dodoString value;                                                                                                       ///< value of the node

						bool CDATA;                                                                                                           ///< true if node doesn't have contents : `<node />`
				};
			};
		};
	};
};

#endif

