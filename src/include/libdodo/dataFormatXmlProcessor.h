/***************************************************************************
 *            dataFormatXmlProcessor.h
 *
 *  Tue Nov 29 2005
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

#ifndef _DATAFORMATXMLPROCESSOR_H_
#define _DATAFORMATXMLPROCESSOR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace io {
        class channel;
    };

    namespace data {
        namespace format {
            namespace xml {
                class node;
                struct __node__;
                struct __doc__;

                /**
                 * @struct __definition__
                 * @brief defines processor tree definition
                 */
                struct __definition__ {
                    /**
                     * constructor
                     */
                    __definition__();

                    /**
                     * @param name defines node name
                     * @param ns defines namespace of the node
                     */
                    __definition__(const dodoString &name,
                                   const            dodoString &ns = __dodostring__);

                    dodoString                          name;               ///< name of the node [[tag]]; if empty - for first - gets root, for children - all[but if children do not have in definition own children]

                    dodoMap<dodoString, __definition__> children;           ///< children definitions
                    bool                                allChildren;        ///< if true - get all children tree[true by default]

                    dodoStringArray                     attributes;         ///< node attrributes; if empty - take all
                    bool                                allAttributes;      ///< if true - get all attributes[true by default]

                    dodoString                          ns;                 ///< node namespace; if empty parser skips namespace specification
                };

                /**
                 * @struct __info__
                 * @brief desribes information about given XML document
                 */
                struct __info__ {
                    /**
                     * constructor
                     */
                    __info__();

                    /**
                     * constructor
                     * @note initializes with user values
                     */
                    __info__(const dodoString &version,
                             const            dodoString &encoding,
                             const            dodoString &root,
                             int              compression);

                    dodoString version;                                     ///< version of XML document
                    dodoString encoding;                                    ///< encoding of XML document
                    dodoString root;                                        ///< name of the root element of XML document

                    int        compression;                                 ///< compression of XML document
                };

                /**
                 * @class processor
                 * @brief provides XML parsing
                 */
                class processor {
                  private:

                    /**
                     * copy constructor
                     * @note to prevent copying
                     */
                    processor(processor &);

                  public:

                    /**
                     * constructor
                     */
                    processor();

                    /**
                     * destructor
                     */
                    ~processor();

                    /**
                     * parse XML file using __definition__ XML definition
                     * @return parsed XML in node structure
                     * @param definition defines structure of XML
                     * @param io defines input source that contains XML
                     */
                    node process(const __definition__ &definition,
                                 const io::channel    &io);

                    /**
                     * parse XML file
                     * @return parsed XML in node structure
                     * @param io defines input source that contains XML
                     */
                    node process(const io::channel &io);

                    bool icaseNames; ///< if true ignore case of names[false by default]

                    /**
                     * @return XML info
                     */
                    __info__ information();

                    /**
                     * free internal structures
                     */
                    void clear();

                    /**
                     * @return XML string
                     * @param root defines root node of XML document
                     * @param encoding defines XML encoding
                     * @param version defines XML version
                     * @param io defines output destination for XML
                     */
                    void make(const node        &root,
                              const dodoString  &encoding,
                              const dodoString  &version,
                              const io::channel &io)  const;

                    /**
                     * @return XML string
                     * @param xnode defines root node of XML document
                     * @param io defines output destination for XML
                     */
                    void make(const node        &xnode,
                              const io::channel &io) const;

                  private:

                    /**
                     * clear parameters of the give node
                     * @param xnode defines node to clean
                     */
                    static void initNode(node &xnode);


                    /**
                     * parse XML using __definition__ XML definition
                     * @return parsed XML in node structure
                     * @param definition defines structure of XML
                     */
                    node parse(const __definition__ &definition);

#ifdef LIBXML2_EXT
                    /**
                     * @return true if content is in CDATA
                     * @param xnode defines XML tree node
                     */
                    bool isCDATA(const __node__ &xnode);

                    /**
                     * parse XML
                     * @return parsed XML in node structure
                     * @param xnode defines XML tree node
                     */
                    dodoArray<node> parse(__node__ node);

                    /**
                     * parses XML using __definition__ XML definition
                     * @return parsed XML nodes in node structures
                     * @param definition defines structure of XML
                     * @param chxnode defines XML tree node
                     */
                    dodoArray<node> parse(const __definition__ &definition,
                                          const __node__       &xnode);

                    /**
                     * get node attributes
                     * @param xnode defines node content
                     * @param attributes defines buffer for attributes
                     */
                    void getAttributes(const __node__ &xnode,
                                       dodoStringMap  &attributes);

                    /**
                     * get node attributes
                     * @param definition defines structure of XML
                     * @param xnode defines node content
                     * @param attributes defines buffer for attributes
                     */
                    void getAttributes(const __definition__ &definition,
                                       const __node__       &xnode,
                                       dodoStringMap        &attributes);

                    /**
                     * get node info
                     * @param xnode defines node content
                     * @param sample defines buffer for node
                     */
                    void getNode(const __node__ &xnode,
                                 node           &sample);
#endif
#ifdef LIBXML2_EXT
                    /**
                     * @typedef strCmp
                     * @brief defines name compare function
                     * @param one defines string to compare
                     * @param two defines string to compare
                     */
                    typedef int (*strCmp)(const unsigned char *one, const unsigned char *two);
                    strCmp cmpFunc;     ///< name compare function

                    /**
                     * find node by definition
                     * @param definition defines structure of XML
                     * @param xnode defines node content
                     */
                    __node__ findNode(const __definition__ &definition,
                                      const __node__       &xnode);

                    __doc__ *document;  ///< XML Document
#endif

                    /**
                     * @enum processorStatementEnum defines processor statements
                     * @note defines positions of string representation in 'statements' class property
                     */
                    enum processorStatementEnum {
                        STATEMENT_LT = 0,
                        STATEMENT_COLON,
                        STATEMENT_SPACE,
                        STATEMENT_XMLNS,
                        STATEMENT_EQUALDQUOTE,
                        STATEMENT_DQUOTE,
                        STATEMENT_SLASHGT,
                        STATEMENT_GT,
                        STATEMENT_CDATAOPEN,
                        STATEMENT_CDATACLOSE,
                        STATEMENT_LTSLASH,

                        STATEMENT_ENUMSIZE
                    };

                    static const dodoString statements[STATEMENT_ENUMSIZE]; ///< processor statements

                    __info__ info;
                };
            };
        };
    };
};
#endif

