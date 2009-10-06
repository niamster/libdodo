/***************************************************************************
 *            dataFormatXmlNode.cc
 *
 *  Wed Nov 30 2005
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

#include <libdodo/directives.h>

#include <libdodo/dataFormatXmlNode.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/types.h>

using namespace dodo::data::format::xml;

node::node() : CDATA(false)
{
}

//-------------------------------------------------------------------

node::node(const dodoString    &name,
		   const dodoStringMap &attributes,
		   const dodoString    &value,
		   const dodoString    &a_ns) : attributes(attributes),
										name(name),
										nodeValue(value),
										CDATA(false)
{
	ns.prefix = a_ns;
}

//-------------------------------------------------------------------

void
node::addChild(const node &child)
{
	nodeChildren[child.name].push_back(child);
}

//-------------------------------------------------------------------

void
node::setChildren(const dodoArray<node> &a_children)
{
	dodoArray<node>::const_iterator i = a_children.begin(), j = a_children.end();
	for (; i != j; ++i)
		nodeChildren[i->name].push_back(*i);
}

//-------------------------------------------------------------------

dodoArray<node>
node::children(const dodoString &name,
				  bool             recursive)
{
	dodoArray<node> nodes = nodeChildren[name];

	if (recursive) {
		dodoArray<node> subnodes;

		dodoArray<node>::iterator i = nodes.begin(), j = nodes.end();
		for (; i != j; ++i) {
			subnodes = i->children(name, true);

			nodes.insert(nodes.end(), subnodes.begin(), subnodes.end());
		}
	}

	return nodes;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
node::childrenNames(bool recursive)
{
	dodoStringArray names;

	if (recursive) {
		dodoStringArray subnames;
		dodoStringArray::iterator x, y;

		dodoArray<node>::iterator o, p;

		dodoMap<dodoString, dodoArray<node>, dodoMapStringCompare>::iterator i = nodeChildren.begin(), j = nodeChildren.end();
		for (; i != j; ++i) {
			o = i->second.begin();
			p = i->second.end();
			for (; o != p; ++o) {
				if (!tools::misc::isInArray(names, o->name))
					names.push_back(o->name);

				subnames = o->childrenNames(true);

				x = subnames.begin();
				y = subnames.end();
				for (; x != y; ++x)
					if (!tools::misc::isInArray(names, *x))
						names.push_back(*x);
			}
		}
	} else {
		dodoMap<dodoString, dodoArray<node>, dodoMapStringCompare>::iterator i = nodeChildren.begin(), j = nodeChildren.end();
		for (; i != j; ++i)
			names.push_back(i->first);
	}

	return names;
}

//-------------------------------------------------------------------

dodoString
node::operator[](const dodoString &name)
{
	return attributes[name];
}

//-------------------------------------------------------------------

void
node::setValue(const dodoString &a_value,
			   bool             a_CDATA)
{
	CDATA = a_CDATA;
	nodeValue = a_value;
}

//-------------------------------------------------------------------

dodoString
node::value()
{
	return nodeValue;
}

//-------------------------------------------------------------------
