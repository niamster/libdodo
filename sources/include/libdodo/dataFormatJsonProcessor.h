/***************************************************************************
 *            dataFormatJsonProcessor.h
 *
 *  Mon Oct 20 2007
 *  Copyright  2007  Dmytro Milinevskyy
 *  milinevskyy@gmail.com
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
#define _DATAFORMATJSONPROCESSOR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace io {
        class channel;
    };

    namespace data {
        namespace format {
            namespace json {
                class node;

                /**
                 * @class processor
                 * @brief provides JSON operations
                 */
                class processor {
                  public:

                    /**
                     * contructor
                     */
                    processor();

                    /**
                     * destructor
                     */
                    ~processor();

                    /**
                     * @param root defines root of unserialized JSON object
                     * @param io defines output destination for serialized JSON object
                     */
                    void make(const node        &root,
                              const io::channel &io);

                    /**
                     * @return root of unserialized JSON object
                     * @param io defines input source that contains serialized JSON object
                     */
                    node process(const io::channel &io);

                    /**
                     * @param root defines root of unserialized JSON object
                     * @param io defines output destination for serialized JSON object
                     */
                    void fromMap(const dodoStringMap &root,
                                 const io::channel   &io);

                    /**
                     * @param io defines IO source that contains serialized JSON object
                     */
                    dodoStringMap toMap(const io::channel &io);

                  private:

                    /**
                     * process string value
                     * @return position of last processed symbol of serialized JSON string object
                     * @param node defines buffer for unserialized JSON string
                     * @param root defines serialized JSON string object
                     * @param pos defines start position for parsing in serialized JSON string object
                     */
                    unsigned long processString(dodo::string       &node,
                                                const dodo::string &root,
                                                unsigned long    pos);

                    /**
                     * process array value
                     * @return position of last processed symbol of serialized JSON string object
                     * @param node defines buffer for unserialized JSON array
                     * @param root defines serialized JSON string object
                     * @param pos defines start position for parsing in serialized JSON string object
                     */
                    unsigned long processArray(dodoArray<node>  &node,
                                               const dodo::string &root,
                                               unsigned long    pos);

                    /**
                     * process object value
                     * @return position of last processed symbol of serialized JSON string object
                     * @param node defines buffer for unserialized JSON object
                     * @param root defines serialized JSON string object
                     * @param pos defines start position for parsing in serialized JSON string object
                     */
                    unsigned long processObject(dodoMap<dodo::string, node, dodoMapStringCompare> &node,
                                                const dodo::string &root,
                                                unsigned long pos);

                    /**
                     * process json value
                     * @return position of last processed symbol of serialized JSON string object
                     * @param node defines buffer for unserialized JSON object
                     * @param root defines serialized JSON string object
                     * @param pos defines start position for parsing in serialized JSON string object
                     */
                    unsigned long processValue(node             &node,
                                               const dodo::string &root,
                                               unsigned long    pos);

                    /**
                     * process json boolean value
                     * @return position of last processed symbol of serialized JSON string object
                     * @param node defines buffer for unserialized JSON boolean
                     * @param root defines serialized JSON string object
                     * @param pos defines start position for parsing in serialized JSON string object
                     */
                    unsigned long processBoolean(bool             &node,
                                                 const dodo::string &root,
                                                 unsigned long    pos);

                    /**
                     * processes json numeric value
                     * @return position of last processed symbol of serialized JSON string object
                     * @param node defines buffer for unserialized JSON numeric
                     * @param root defines serialized JSON string object
                     * @param pos defines start position for parsing in serialized JSON string object
                     */
                    unsigned long processNumeric(long             &node,
                                                 const dodo::string &root,
                                                 unsigned long    pos);

                    /**
                     * process json null value
                     * @return position of last processed symbol of serialized JSON string object
                     * @param root defines serialized JSON string object
                     * @param pos defines start position for parsing in serialized JSON string object
                     */
                    unsigned long processNull(const dodo::string &root,
                                              unsigned long    pos);

                    /**
                     * @enum stateEnum describes states for json processor
                     */
                    enum stateEnum {
                        STATE_OBJECT_INITIAL,
                        STATE_OBJECT_OBJECTNAME,
                        STATE_OBJECT_OBJECTVALUE,
                    };
                };
            };
        };
    };
};
#endif
