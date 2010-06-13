/***************************************************************************
 *            dataTplProcessor.h
 *
 *  Tue Jan 24 2006
 *  Copyright  2005  Dmytro Milinevskyy
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

#ifndef _DATATPLPROCESSOR_H_
#define _DATATPLPROCESSOR_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace io {
        class channel;
    };

    namespace data {
        namespace tpl {
            /**
             * @class processor
             * @brief parses template
             */
            class processor {
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
                 * @param path defines path of template
                 * @param tpl defines stream for outputting parsed template
                 */
                void processFile(const dodo::string &path,
                                 io::channel      &tpl);

                /**
                 * @param buffer defines buffer where template is stored
                 * @param tpl defines stream for outputting parsed template
                 */
                void processString(const dodo::string &buffer,
                                   io::channel      &tpl);

                /**
                 * set variable
                 * @param varName defines name of variable
                 * @param varVal defines value of variable
                 */
                void assign(dodo::string       varName,
                            const dodo::string &varVal);

                /**
                 * set variable
                 * @param varName defines name of variable
                 * @param varVal defines value of variable(array)
                 */
                void assign(dodo::string            varName,
                            const dodoStringArray &varVal);

                /**
                 * set variable
                 * @param varName defines name of variable
                 * @param varVal defines value of variablee(hash)
                 */
                void assign(dodo::string          varName,
                            const dodoStringMap &varVal);

                /**
                 * set variable
                 * @param varName defines name of variable
                 * @param varVal defines value of variable(array of hashes)
                 */
                void assign(dodo::string                     varName,
                            const dodoArray<dodoStringMap> &varVal);

                /**
                 * clear internal data[assigned vars, etc.]
                 */
                void clear();

                dodo::string basePath; ///< base path for templates[if empty - full/relative path must be defined in preProcess/include]

              private:

                /**
                 * @return preprocessed template
                 * @param path defines path of template
                 */
                dodo::string preProcessFile(const dodo::string &path);

                /**
                 * @return preprocessed template
                 * @param buffer defines buffer where template is stored
                 */
                dodo::string preProcessString(const dodo::string &buffer);

                /**
                 * @return parsed template from processored buffer
                 * @param buffer defines buffer where template is stored
                 * @param path defines path of template
                 * @param tpl defines stream for outputting parsed template
                 */
                void _processString(const dodo::string &buffer,
                                    const dodo::string &path,
                                    io::channel      &tpl);

                /**
                 * process `if` statement
                 * @return position of cursor where to continue processing
                 * @param buffer defines buffer where template is stored
                 * @param start defines position in buffer after ')>' of `<( if ... )>` block
                 * @param statement defines `if` statement
                 * @param tpl defines stream for outputting parsed template
                 * @param path defines path of template
                 */
                unsigned long _if(const dodo::string &buffer,
                                  unsigned long    start,
                                  const dodo::string &statement,
                                  io::channel      &tpl,
                                  const dodo::string &path);

                /**
                 * process `for` statement
                 * @return position of cursor where to continue processing
                 * @param buffer defines buffer where template is stored
                 * @param start defines position in buffer after ')>' of `<( for ... )>` block
                 * @param statement defines `for` statement
                 * @param tpl defines stream for outputting parsed template
                 * @param path defines path of template
                 */
                unsigned long _for(const dodo::string &buffer,
                                   unsigned long    start,
                                   const dodo::string &statement,
                                   io::channel      &tpl,
                                   const dodo::string &path);

                /**
                 * process `for` statement
                 * @return position of cursor where to continue processing
                 * @param buffer defines buffer where template is stored
                 * @param start defines position in buffer after ')>' of `<( namespace ... )>` block
                 * @param tpl defines stream for outputting parsed template
                 * @param path defines path of template
                 */
                unsigned long _ns(const dodo::string &buffer,
                                  unsigned long    start,
                                  io::channel      &tpl,
                                  const dodo::string &path);

                /**
                 * process `print` statement
                 * @return position of cursor where to continue processing
                 * @param start defines position in buffer after ')>' of `<( print ... )>` block
                 * @param statement defines `print` statement
                 * @param tpl defines stream for outputting parsed template
                 * @param path defines path of template
                 */
                unsigned long _print(unsigned long    start,
                                     const dodo::string &statement,
                                     io::channel      &tpl,
                                     const dodo::string &path);

                /**
                 * process `break` statement
                 * @return true if process must break
                 * @param start defines position in buffer after ')>' of `<( break ... )>` block
                 * @param statement defines `break` statement
                 * @param path defines path of template
                 */
                bool _break(unsigned long    start,
                            const dodo::string &statement,
                            const dodo::string &path);

                /**
                 * process `assign` statement
                 * @return position of cursor where to continue processing
                 * @param start defines position in buffer after ')>' of `<( assign ... )>` block
                 * @param statement defines `assign` statement
                 * @param path defines path of template
                 */
                unsigned long _assign(unsigned long    start,
                                      const dodo::string &statement,
                                      const dodo::string &path);

                /**
                 * process `include` statement
                 * @return position of cursor where to continue processing
                 * @param start defines position in buffer after ')>' of `<( include ... )>` block
                 * @param statement defines `include` statement
                 * @param processed indicates string where to add result
                 * @param path defines path of template
                 */
                unsigned long _include(unsigned long    start,
                                       const dodo::string &statement,
                                       io::channel      &tpl,
                                       const dodo::string &path);

                /**
                 * clean namespace variable and bring back to life vars of prevous namespace that were overwritten
                 */
                void cleanNamespace();

                /**
                 * @return position in buffer after ')>' of the end block
                 * @param buffer defines buffer where template is stored
                 * @param start defines position in buffer after ')>' of the block
                 * @param st defines open statement[if, for ...]
                 * @param ts defines close statement[fi, rof ...]
                 * @param path defines path of template
                 */
                unsigned long blockEnd(const dodo::string &buffer,
                                       unsigned long    start,
                                       const dodo::string &st,
                                       const dodo::string &ts,
                                       const dodo::string &path);

                /**
                 * @return value of the variable
                 * @param varName defines name of variable
                 * @param start defines position in file
                 * @param path defines path of template
                 */
                dodo::string getVar(const dodo::string &varName,
                                  unsigned long    start,
                                  const dodo::string &path);

                /**
                 * @return interpolated name of the variable[expanded from {}]
                 * @param varName defines name of variable
                 * @param start defines position in file
                 * @param path defines path of template
                 */
                dodo::string getVarName(const dodo::string &varName,
                                      unsigned long    start,
                                      const dodo::string &path);

                /**
                 * @return extracted data(e.g. removes pairs of ",',`)
                 * @param statement defines statement that needs extraction from the pairs of ",',`
                 */
                dodo::string trim(const dodo::string &statement);

                /**
                 * @return preprocessed template
                 * @param buffer defines buffer where template is stored
                 * @param path defines path of template
                 */
                dodo::string _preProcessString(const dodo::string &buffer,
                                             const dodo::string &path);

                /**
                 * @return line number in file
                 * @param newlines defines positions of new lines if template
                 * @param pos defines position of symbol in template
                 */
                unsigned long getLineNumber(const dodoArray<unsigned long> &newlines,
                                            unsigned long                  pos);

                /**
                 * @return positions of new lines
                 * @param buffer defines buffer where template is stored
                 */
                dodoArray<unsigned long> detectNewLines(const dodo::string &buffer);

                dodoArray< dodoArray<unsigned long> > newLinePositions;                 ///< stack of positions of new lines of templates

                dodoStringList processed;                                               ///< files that will be skipped due to the recursion

                dodoStringMap global;                                                   ///< set of global variables[user-set]
                dodoMap<dodo::string, dodoStringArray> globalArray;                       ///< global user-defined variables(arrays)
                dodoMap<dodo::string, dodoStringMap> globalHash;                          ///< global user-defined variables(hashes)
                dodoMap<dodo::string, dodoArray<dodoStringMap> > globalArrayHash;         ///< global user-defined variables(array of hashes)

                dodoMap<unsigned int, dodoStringMap> local;                             ///< local variables[occur in `for` block]
                dodoMap<unsigned int, dodoMap<dodo::string, dodoStringMap> > localHash;   ///< local variables(hashes)[occur in `for` block]

                dodoStringMap dodo;                                                     ///< auxillary variables[dodo defined][for dodo.*]

                bool continueFlag;                                                      ///< indicates `continue` state

                unsigned int breakDeepness;                                             ///< deepness of the break
                unsigned int loopDeepness;                                              ///< deepness of the loop

                unsigned long iterator;                                                 ///< amount of iterations of the loop

                unsigned int namespaceDeepness;                                         ///< cgi object through what output will be performed

                /**
                 * @enum processorStatementEnum defines processor statements
                 * @note defines positions of string representation in 'statements' class property
                 */
                enum processorStatementEnum {
                    STATEMENT_DODO = 0,
                    STATEMENT_OPEN_ST,
                    STATEMENT_CLOSE_ST,
                    STATEMENT_OPEN_NP,
                    STATEMENT_CLOSE_NP,
                    STATEMENT_OPEN_COMM,
                    STATEMENT_CLOSE_COMM,
                    STATEMENT_OPEN_IF,
                    STATEMENT_ELSE,
                    STATEMENT_CLOSE_IF,
                    STATEMENT_OPEN_FOR,
                    STATEMENT_IN,
                    STATEMENT_KEY_VALUE,
                    STATEMENT_CLOSE_FOR,
                    STATEMENT_PRINT,
                    STATEMENT_BREAK,
                    STATEMENT_CONT,
                    STATEMENT_ASSIGN,
                    STATEMENT_ASSIGN_OP,
                    STATEMENT_OPEN_NS,
                    STATEMENT_CLOSE_NS,
                    STATEMENT_INCLUDE,
                    STATEMENT_ITERATOR,
                    STATEMENT_VERSION,
                    STATEMENT_DOT,
                    STATEMENT_COMA,
                    STATEMENT_DOLLAR,
                    STATEMENT_FALSE,
                    STATEMENT_OPEN_VARPART,
                    STATEMENT_CLOSE_VARPART,

                    STATEMENT_ENUMSIZE
                };

                static const dodo::string statements[STATEMENT_ENUMSIZE]; ///< processor statements[for dodo.*, ...]
            };
        };
    };
};
#endif
