/***************************************************************************
 *            toolsNetwork.h
 *
 *  Thu Oct 04 2005
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

#ifndef _TOOLSNETWORK_H_
#define _TOOLSNETWORK_H_ 1

#include <libdodo/directives.h>

#include <libdodo/types.h>

namespace dodo {
    namespace tools {
        /**
         * @class network
         * @brief provides information about network environment
         */
        class network {
          public:

            /**
             * @struct __interface__
             * @brief defines interface information
             */
            struct __interface__ {
                dodo::string address;         ///< ip address of the interface
                dodo::string broadcast;       ///< broadcast address of the interface
                dodo::string netmask;         ///< netmask of the interface
                dodo::string hwaddr;          ///< harware address of the interface(MAC)
                bool       up;              ///< true if interface is up
                bool       loop;            ///< true if interface is a loopback
            };

            /**
             * @struct __host__
             * @brief defines information about host
             */
            struct __host__ {
                dodo::string      name;       ///< original name of the host
                dodoStringArray aliases;    ///< aliases of the host
                dodoStringArray addresses;  ///< addresses of the host
            };

            /**
             * @struct __service__
             * @brief defines info about service
             */
            struct __service__ {
                dodo::string      name;       ///< original name of the service
                dodoStringArray aliases;    ///< aliases of the service
                int             port;       ///< port of the service
            };

            /**
             * @return a list of interfaces
             */
            static dodoStringArray interfacesNames();

            /**
             * @return information about the interface
             * @param interface defines a name of the interface
             */
            static __interface__ interface(const dodo::string &interface);

            /**
             * @return information about the given host
             * @param host defines a name of the host
             */
            static __host__ host(const dodo::string &host);

            /**
             * @return primary host ip
             * @param host defines a name of the host
             */
            static dodo::string hostPrimaryIp(const dodo::string &host);

            /**
             * @return name of the local host
             */
            static dodo::string localName();

            /**
             * set local host name
             * @param host defines name of the host
             */
            static void setLocalName(const dodo::string &host);

            /**
             * @return information about the service
             * @param service defices name of the service
             * @param protocol defines protocol of the service(tcp, udp ..)
             */
            static __service__ service(const dodo::string &service,
                                       const dodo::string &protocol);

            /**
             * @return information about the service
             * @param port defices port of the service
             * @param protocol defines protocol of the service(tcp, udp ..)
             */
            static __service__ service(int              port,
                                       const dodo::string &protocol);

            /**
             * send mail using sendmail external program
             * @param to defines mail address[possible multiply separated with coma]
             * @param subject defines a subject of the letter;for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`
             * @param message defines a message to send
             * @param headers defines extra headers
             * @param path defines path to sendmail
             */
            static void mail(const dodo::string &to,
                             const dodo::string &subject,
                             const dodo::string &message,
                             const dodo::string &headers = __dodostring__,
                             const dodo::string &path = "/usr/sbin/sendmail");

            /**
             * send mail
             * @param host defines host of smtp server(ip)
             * @param port defines port of smtp server
             * @param to defines mail address where to send[possible multiply separated with coma]
             * @param from defines mail address of sender
             * @param subject defines a subject of the letter;for utf should use: `'=?utf-8?B?'.encodeBase64(subject).'?='`
             * @param message defines a message to send
             * @param login defines a login for auth
             * @param pass defines a password for auth
             * @param headers defines extra headers[each must ends with CR NR]
             * @note if login is emty no auth is performed
             */
            static void mail(const dodo::string &host,
                             int              port,
                             const dodo::string &to,
                             const dodo::string &from,
                             const dodo::string &subject,
                             const dodo::string &message,
                             const dodo::string &login = __dodostring__,
                             const dodo::string &pass = __dodostring__,
                             const dodo::string &headers = __dodostring__);
        };
    };
};
#endif
