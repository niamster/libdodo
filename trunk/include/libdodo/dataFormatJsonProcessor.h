/***************************************************************************
 *            dataFormatJsonProcessor.h
 *
 *  Mon Oct 20 02:15:55 2007
 *  Copyright  2007  Ni@m
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

#ifndef _DATAFORMATJSONPROCESSOR_H_
#define _DATAFORMATJSONPROCESSOR_H_

#include <libdodo/directives.h>

#include <libdodo/types.h>
#include <libdodo/toolsString.h>
#include <libdodo/toolsFilesystem.h>
#include <libdodo/dataFormatJsonProcessorEx.h>
#include <libdodo/dataFormatJsonNode.h>

namespace dodo
{
	namespace data
	{
		namespace format
		{
			namespace json
			{
				/**
				 * @class processor
				 * @brief provides JSON operations
				 */
				class processor
				{
					public:

						/**
						 * contructor
						 */
						processor();

						/**
						 * destructor
						 */
						virtual ~processor();

						/**
						 * @return string that contains serialized JSON object
						 * @param root defines root of unserialized JSON object
						 */
						virtual dodoString make(const node &root);

						/**
						 * @return root of unserialized JSON object
						 * @param root defines string that contains serialized JSON object
						 */
						virtual node processString(const dodoString &root);

						/**
						 * @return root of unserialized JSON object
						 * @param path defines path to file that contains serialized JSON object
						 */
						virtual node processFile(const dodoString &path);

						/**
						 * @return string that contains serialized JSON object
						 * @param root defines root of unserialized JSON object
						 */
						virtual dodoString fromMap(const dodoStringMap &root);

						/**
						 * @return root of unserialized JSON object
						 * @param root defines string that contains serialized JSON object
						 */
						virtual dodoStringMap toMap(const dodoString &root);

					protected:

						/**
						 * process string value
						 * @return position of last processed symbol of serialized JSON string object
						 * @param node defines buffer for unserialized JSON string
						 * @param root defines serialized JSON string object
						 * @param pos defines start position for parsing in serialized JSON string object
						 */
						virtual unsigned long processString(dodoString &node, const dodoString &root, unsigned long pos);

						/**
						 * process array value
						 * @return position of last processed symbol of serialized JSON string object
						 * @param node defines buffer for unserialized JSON array
						 * @param root defines serialized JSON string object
						 * @param pos defines start position for parsing in serialized JSON string object
						 */
						virtual unsigned long processArray(dodoArray<node> &node, const dodoString &root, unsigned long pos);

						/**
						 * process object value
						 * @return position of last processed symbol of serialized JSON string object
						 * @param node defines buffer for unserialized JSON object
						 * @param root defines serialized JSON string object
						 * @param pos defines start position for parsing in serialized JSON string object
						 */
						virtual unsigned long processObject(dodoMap<dodoString, node, dodoMapStringCompare> &node, const dodoString &root, unsigned long pos);

						/**
						 * process json value
						 * @return position of last processed symbol of serialized JSON string object
						 * @param node defines buffer for unserialized JSON object
						 * @param root defines serialized JSON string object
						 * @param pos defines start position for parsing in serialized JSON string object
						 */
						virtual unsigned long processValue(node &node, const dodoString &root, unsigned long pos);

						/**
						 * process json boolean value
						 * @return position of last processed symbol of serialized JSON string object
						 * @param node defines buffer for unserialized JSON boolean
						 * @param root defines serialized JSON string object
						 * @param pos defines start position for parsing in serialized JSON string object
						 */
						virtual unsigned long processBoolean(bool &node, const dodoString &root, unsigned long pos);

						/**
						 * processes json numeric value
						 * @return position of last processed symbol of serialized JSON string object
						 * @param node defines buffer for unserialized JSON numeric
						 * @param root defines serialized JSON string object
						 * @param pos defines start position for parsing in serialized JSON string object
						 */
						virtual unsigned long processNumeric(long &node, const dodoString &root, unsigned long pos);

						/**
						 * process json null value
						 * @return position of last processed symbol of serialized JSON string object
						 * @param root defines serialized JSON string object
						 * @param pos defines start position for parsing in serialized JSON string object
						 */
						virtual unsigned long processNull(const dodoString &root, unsigned long pos);

						/**
						 * @enum jsonStateEnum describes states for json processor
						 */
						enum jsonStateEnum
						{
							JSON_STATE_OBJECT_INITIAL,
							JSON_STATE_OBJECT_OBJECTNAME,
							JSON_STATE_OBJECT_OBJECTVALUE,
						};
				};
			};
		};
	};
};

#endif
