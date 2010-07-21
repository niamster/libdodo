/***************************************************************************
 *            toolsNetwork.cc
 *
 *  Thu Sep 20 2005
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

#include <libdodo/directives.h>

#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

#include <libdodo/toolsNetwork.h>
#include <libdodo/toolsNetworkEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/toolsString.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioNetworkClient.h>
#include <libdodo/toolsCode.h>

using namespace dodo::tools;

network::__host__
network::host(const dodo::string &host)
{
    hostent *ent = gethostbyname(host.data());

    if (ent == NULL)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_HOST, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

    __host__ info;
    info.name = ent->h_name;

    int i(0);

    while (ent->h_aliases[i] != NULL)
        info.aliases.push_back(ent->h_aliases[i++]);

    i = 0;
    char temp[INET6_ADDRSTRLEN];

    while (ent->h_addr_list[i] != NULL) {
        switch (ent->h_addrtype) {
            case AF_INET:

                if (inet_ntop(AF_INET, ent->h_addr_list[i], temp, INET_ADDRSTRLEN) == NULL) {
                    ++i;

                    continue;
                }

                break;

            case AF_INET6:

                if (inet_ntop(AF_INET6, ent->h_addr_list[i], temp, INET6_ADDRSTRLEN) == NULL) {
                    ++i;

                    continue;
                }

                break;
        }

        info.addresses.push_back(temp);
        ++i;
    }

    return info;
}

//-------------------------------------------------------------------

dodo::string
network::hostPrimaryIp(const dodo::string &host)
{
    hostent *ent = gethostbyname(host.data());

    if (ent == NULL)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_HOSTPRIMARYIP, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

    char temp[INET6_ADDRSTRLEN];

    if (ent->h_addr_list[0] != NULL) {
        switch (ent->h_addrtype) {
            case AF_INET:

                if (inet_ntop(AF_INET, ent->h_addr_list[0], temp, INET_ADDRSTRLEN) == NULL)
                    throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_HOSTPRIMARYIP, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

                break;

            case AF_INET6:

                if (inet_ntop(AF_INET6, ent->h_addr_list[0], temp, INET6_ADDRSTRLEN) == NULL)
                    throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_HOSTPRIMARYIP, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

                break;
        }
    }

    return temp;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
network::interfacesNames()
{
    struct if_nameindex *ifaces = if_nameindex();
    if (ifaces == NULL)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACESNAMES, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    int i(-1);
    dodoStringArray arr;

    while (ifaces[++i].if_index != 0)
        arr.push_back(ifaces[i].if_name);

    if_freenameindex(ifaces);

    return arr;
}

//-------------------------------------------------------------------

network::__service__
network::service(const dodo::string &host,
                 const dodo::string &protocol)
{
    servent *ent = getservbyname(host.data(), protocol.data());

    __service__ info;

    if (ent == NULL)
        return info;

    info.name = ent->s_name;
    info.port = ent->s_port;

    int i(0);

    while (ent->s_aliases[i] != NULL)
        info.aliases.push_back(ent->s_aliases[i++]);

    return info;
}

//-------------------------------------------------------------------

network::__service__
network::service(int              port,
                 const dodo::string &protocol)
{
    servent *ent = getservbyport(port, protocol.data());

    __service__ info;

    if (ent == NULL)
        return info;

    info.name = ent->s_name;
    info.port = ent->s_port;

    int i(0);

    while (ent->s_aliases[i] != NULL)
        info.aliases.push_back(ent->s_aliases[i++]);

    return info;
}

//-------------------------------------------------------------------

network::__interface__
network::interface(const dodo::string &interface)
{
    int socket = ::socket(PF_INET, SOCK_DGRAM, 0);
    if (socket == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    ifreq ifr;
    strcpy(ifr.ifr_name, interface.data());

    __interface__ info;
    char add[INET6_ADDRSTRLEN];

    sockaddr_in sin;

    if (::ioctl(socket, SIOCGIFADDR, &ifr) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    memcpy((void *)&sin, &ifr.ifr_ifru.ifru_addr, sizeof(sockaddr));

    if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
        info.address = add;

#ifdef __FreeBSD__
#else
    if (::ioctl(socket, SIOCGIFNETMASK, &ifr) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    memcpy((void *)&sin, &ifr.ifr_ifru.ifru_netmask, sizeof(sockaddr));

    if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
        info.netmask = add;

#endif

    if (::ioctl(socket, SIOCGIFBRDADDR, &ifr) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    memcpy((void *)&sin, &ifr.ifr_ifru.ifru_broadaddr, sizeof(sockaddr));

    if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
        info.broadcast = add;

#ifdef __FreeBSD__
#else
    if (::ioctl(socket, SIOCGIFHWADDR, &ifr) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    sprintf(add, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", ifr.ifr_ifru.ifru_hwaddr.sa_data[0] & 0xff,
            ifr.ifr_ifru.ifru_hwaddr.sa_data[1] & 0xff,
            ifr.ifr_ifru.ifru_hwaddr.sa_data[2] & 0xff,
            ifr.ifr_ifru.ifru_hwaddr.sa_data[3] & 0xff,
            ifr.ifr_ifru.ifru_hwaddr.sa_data[4] & 0xff,
            ifr.ifr_ifru.ifru_hwaddr.sa_data[5] & 0xff);
#endif

    info.hwaddr = add;

    if (::ioctl(socket, SIOCGIFFLAGS, &ifr) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    if (::close(socket) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_INTERFACE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

#ifdef __FreeBSD__
    if (isSetFlag(ifr.ifr_ifru.ifru_flags[0], IFF_LOOPBACK))
        info.loop = true;

    if (isSetFlag(ifr.ifr_ifru.ifru_flags[0], IFF_UP))
        info.up = true;

#else
    if (isSetFlag(IFF_LOOPBACK & ifr.ifr_ifru.ifru_flags, IFF_LOOPBACK))
        info.loop = true;

    if (isSetFlag(IFF_UP & ifr.ifr_ifru.ifru_flags, IFF_UP))
        info.up = true;

#endif

    return info;
}

//-------------------------------------------------------------------

dodo::string
network::localName()
{
    dodo::string temp0;
    char *temp1 = new char[256];

    if (::gethostname(temp1, 255) == -1) {
        delete [] temp1;

        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_LOCALNAME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
    }

    temp0 = dodo::string(temp1);

    delete [] temp1;

    return temp0;
}

//-------------------------------------------------------------------

void
network::setLocalName(const dodo::string &host)
{
    if (::sethostname(host.data(), host.size()) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_SETLOCALNAME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
network::mail(const dodo::string &to,
              const dodo::string &subject,
              const dodo::string &message,
              const dodo::string &headers,
              const dodo::string &path)
{
    FILE *sendmail = popen((path + " " + to).data(), "w");

    if (sendmail == NULL)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

    fprintf(sendmail, "To: %s\n", to.data());
    fprintf(sendmail, "Subject: %s\n", subject.data());
    if (headers.size() > 0)
        fprintf(sendmail, "%s\n", headers.data());
    fprintf(sendmail, "\n%s\n", message.data());

    if (pclose(sendmail) == -1)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

void
network::mail(const dodo::string &host,
              int              port,
              const dodo::string &to,
              const dodo::string &from,
              const dodo::string &subject,
              const dodo::string &message,
              const dodo::string &login,
              const dodo::string &pass,
              const dodo::string &headers)
{
    using namespace io::network;

    enum mailAuthEnum {
        SMTPAUTH_CRAMMD5 = 2,
        SMTPAUTH_LOGIN = 4,
        SMTPAUTH_PLAIN = 8
    };

    unsigned short authType = 0;

    bool auth = login.size() > 0 ? true : false;

    short family = connection::PROTOCOL_FAMILY_IPV4;
    if (host.find(":") != dodo::string::POSITION_END)
        family = connection::PROTOCOL_FAMILY_IPV6;

    exchange ex;
    client net(family, connection::TRANSFER_STREAM);

    net.connect(host, port, ex);

    dodo::string mess;

    mess = ex.readString();
    ex.writeString("EHLO " + network::localName() + "\r\n");
    mess = ex.readString();

    if (string::stringToI(dodo::string(mess.data(), 3)) != 250)
        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_LIBDODO, NETWORKEX_BADMAILHELO, TOOLSNETWORKEX_BADMAILHELO_STR, __LINE__, __FILE__);

    if (auth) {
        if (string::contains(mess, "CRAM-MD5"))
            addFlag(authType, SMTPAUTH_CRAMMD5);

        if (string::contains(mess, "LOGIN"))
            addFlag(authType, SMTPAUTH_LOGIN);

        if (string::contains(mess, "PLAIN"))
            addFlag(authType, SMTPAUTH_PLAIN);
    }

    if (auth) {
        if (isSetFlag(authType, SMTPAUTH_CRAMMD5)) {
            ex.writeString("AUTH CRAM-MD5\r\n");
            mess = ex.readString();

            if (string::stringToI(dodo::string(mess.data(), 3)) != 334)
                throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);

            dodo::string ticket = code::decodeBase64(dodo::string(mess.data() + 4, mess.size() - 4));

            dodo::string md5pass;
            if (pass.size() > 64)
                md5pass = code::MD5(pass);
            else
                md5pass = pass;

            unsigned char ipad[65];
            unsigned char opad[65];

            memset(ipad, 0, 65);
            memset(opad, 0, 65);

            memcpy(ipad, md5pass.data(), md5pass.size());
            memcpy(opad, md5pass.data(), md5pass.size());

            for (short i = 0; i < 64; ++i) {
                ipad[i] ^= 0x36;
                opad[i] ^= 0x5c;
            }

            code::__MD5Context__ context;
            unsigned char digest[16];

            code::MD5Init(&context);
            code::MD5Update(&context, ipad, 64);
            code::MD5Update(&context, (unsigned char *)ticket.data(), ticket.size());
            code::MD5Final(digest, &context);

            code::MD5Init(&context);
            code::MD5Update(&context, opad, 64);
            code::MD5Update(&context, digest, 16);
            code::MD5Final(digest, &context);

            md5pass = code::binToHex(dodo::string((char *)&digest, 16));

            ex.writeString(code::encodeBase64(login + " " + md5pass) + "\r\n");
            mess = ex.readString();

            if (string::stringToI(dodo::string(mess.data(), 3)) != 235)
                throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
        } else {
            if (isSetFlag(authType, SMTPAUTH_LOGIN)) {
                ex.writeString("AUTH LOGIN\r\n");
                mess = ex.readString();

                if (string::stringToI(dodo::string(mess.data(), 3)) != 334)
                    throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);

                ex.writeString(code::encodeBase64(login) + "\r\n");
                mess = ex.readString();

                if (string::stringToI(dodo::string(mess.data(), 3)) != 334)
                    throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);

                ex.writeString(code::encodeBase64(pass) + "\r\n");
                mess = ex.readString();

                if (string::stringToI(dodo::string(mess.data(), 3)) != 235)
                    throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
            } else {
                if (isSetFlag(authType, SMTPAUTH_PLAIN)) {
                    ex.writeString("AUTH PLAIN" + code::encodeBase64(login + "\0" + login + "\0" + pass) + "\r\n");
                    mess = ex.readString();

                    if (string::stringToI(dodo::string(mess.data(), 3)) != 334)
                        throw exception::basic(exception::MODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
                }
            }
        }
    }

    ex.writeString("MAIL FROM: <" + from + ">\r\n");
    mess = ex.readString();

    dodoStringArray pock = misc::split(to, ",");

    dodoStringArray::iterator i = pock.begin(), j = pock.end();
    for (; i != j; ++i) {
        ex.writeString("RCPT TO: <" + *i + ">\r\n");
        mess = ex.readString();
    }

    ex.writeString("DATA\r\n");
    mess = ex.readString();

    ex.writeString("To: " + to + "\r\n");
    ex.writeString("From: " + from + "\r\n");
    ex.writeString("X-Mailer: " PACKAGE_NAME "/" PACKAGE_VERSION "\r\n");
    ex.writeString("Subject: " + subject  + "\r\n");
    ex.writeString(headers);
    ex.writeString(message);
    ex.writeString("\r\n.\r\n");
    ex.writeString("QUIT\r\n");
}

//-------------------------------------------------------------------

