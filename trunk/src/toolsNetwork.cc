/***************************************************************************
 *            toolsNetwork.cc
 *
 *  Thu Sep 20 01:43:24 2005
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

#include <libdodo/toolsNetwork.h>
#include <libdodo/toolsNetworkEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/ioNetworkConnection.h>
#include <libdodo/ioNetworkExchange.h>
#include <libdodo/ioNetworkClient.h>
#include <libdodo/toolsCode.h>

using namespace dodo::tools;

__hostInfo__
network::getHostInfo(const dodoString &host)
{
	hostent *ent = gethostbyname(host.c_str());

	if (ent == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETHOSTINFO, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);
	}

	__hostInfo__ info;
	info.name = ent->h_name;

	int i(0);

	while (ent->h_aliases[i] != NULL)
	{
		info.aliases.push_back(ent->h_aliases[i++]);
	}

	i = 0;
	char temp[INET6_ADDRSTRLEN];

	while (ent->h_addr_list[i] != NULL)
	{
		switch (ent->h_addrtype)
		{
			case AF_INET:

				if (inet_ntop(AF_INET, ent->h_addr_list[i], temp, INET_ADDRSTRLEN) == NULL)
				{
					++i;

					continue;
				}

				break;

			case AF_INET6:

				if (inet_ntop(AF_INET6, ent->h_addr_list[i], temp, INET6_ADDRSTRLEN) == NULL)
				{
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

dodoString
network::getHostPrimaryIp(const dodoString &host)
{
	hostent *ent = gethostbyname(host.c_str());

	if (ent == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETHOSTPRIMARYIP, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);
	}

	char temp[INET6_ADDRSTRLEN];

	if (ent->h_addr_list[0] != NULL)
	{
		switch (ent->h_addrtype)
		{
			case AF_INET:

				if (inet_ntop(AF_INET, ent->h_addr_list[0], temp, INET_ADDRSTRLEN) == NULL)
				{
					throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETHOSTPRIMARYIP, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);
				}

				break;

			case AF_INET6:

				if (inet_ntop(AF_INET6, ent->h_addr_list[0], temp, INET6_ADDRSTRLEN) == NULL)
				{
					throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETHOSTPRIMARYIP, exception::ERRNO_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);
				}

				break;
		}
	}

	return temp;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
network::getInterfacesNames()
{
	struct if_nameindex *ifaces = if_nameindex();
	if (ifaces == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACESNAMES, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	int i(-1);
	dodoStringArray arr;

	while (ifaces[++i].if_index != 0)
	{
		arr.push_back(ifaces[i].if_name);
	}

	if_freenameindex(ifaces);

	return arr;
}

//-------------------------------------------------------------------

__serviceInfo__
network::getServiceInfo(const dodoString &host,
						const dodoString &protocol)
{
	servent *ent = getservbyname(host.c_str(), protocol.c_str());

	__serviceInfo__ info;

	if (ent == NULL)
	{
		return info;
	}

	info.name = ent->s_name;
	info.port = ent->s_port;

	int i(0);

	while (ent->s_aliases[i] != NULL)
	{
		info.aliases.push_back(ent->s_aliases[i++]);
	}

	return info;
}

//-------------------------------------------------------------------

__serviceInfo__
network::getServiceInfo(int              port,
						const dodoString &protocol)
{
	servent *ent = getservbyport(port, protocol.c_str());

	__serviceInfo__ info;

	if (ent == NULL)
	{
		return info;
	}

	info.name = ent->s_name;
	info.port = ent->s_port;

	int i(0);

	while (ent->s_aliases[i] != NULL)
	{
		info.aliases.push_back(ent->s_aliases[i++]);
	}

	return info;
}

//-------------------------------------------------------------------

__interfaceInfo__
network::getInterfaceInfo(const dodoString &interface)
{
	int socket = ::socket(PF_INET, SOCK_DGRAM, 0);
	if (socket == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	ifreq ifr;
	strcpy(ifr.ifr_name, interface.c_str());

	__interfaceInfo__ info;
	char add[INET6_ADDRSTRLEN];

	sockaddr_in sin;

	if (::ioctl(socket, SIOCGIFADDR, &ifr) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	memcpy((void *)&sin, &ifr.ifr_ifru.ifru_addr, sizeof(sockaddr));

	if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
	{
		info.address = add;
	}

#ifdef __FreeBSD____
#else
	if (::ioctl(socket, SIOCGIFNETMASK, &ifr) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	memcpy((void *)&sin, &ifr.ifr_ifru.ifru_netmask, sizeof(sockaddr));

	if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
	{
		info.netmask = add;
	}
#endif

	if (::ioctl(socket, SIOCGIFBRDADDR, &ifr) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	memcpy((void *)&sin, &ifr.ifr_ifru.ifru_broadaddr, sizeof(sockaddr));

	if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
	{
		info.broadcast = add;
	}

#ifdef __FreeBSD____
#else
	if (::ioctl(socket, SIOCGIFHWADDR, &ifr) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	sprintf(add, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", ifr.ifr_ifru.ifru_hwaddr.sa_data[0] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[1] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[2] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[3] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[4] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[5] & 0xff);
#endif

	info.hwaddr = add;

	if (::ioctl(socket, SIOCGIFFLAGS, &ifr) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	if (::close(socket) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETINTERFACEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

#ifdef __FreeBSD____
	if (isSetFlag(ifr.ifr_ifru.ifru_flags[0], IFF_LOOPBACK))
	{
		info.loop = true;
	}

	if (isSetFlag(ifr.ifr_ifru.ifru_flags[0], IFF_UP))
	{
		info.up = true;
	}
#else
	if (isSetFlag(IFF_LOOPBACK & ifr.ifr_ifru.ifru_flags, IFF_LOOPBACK))
	{
		info.loop = true;
	}

	if (isSetFlag(IFF_UP & ifr.ifr_ifru.ifru_flags, IFF_UP))
	{
		info.up = true;
	}
#endif

	return info;
}
//-------------------------------------------------------------------

dodoString
network::getLocalName()
{
	dodoString temp0;
	char *temp1 = new char[256];

	if (::gethostname(temp1, 255) == -1)
	{
		delete [] temp1;

		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_GETLOCALNAME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	temp0.assign(temp1);

	delete [] temp1;

	return temp0;
}

//-------------------------------------------------------------------

void
network::setLocalName(const dodoString &host)
{
	if (::sethostname(host.c_str(), host.size()) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_SETLOCALNAME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
network::mail(const dodoString &to,
			  const dodoString &subject,
			  const dodoString &message,
			  const dodoString &headers,
			  const dodoString &path)
{
	FILE *sendmail = popen((path + " " + to).c_str(), "w");

	if (sendmail == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	fprintf(sendmail, "To: %s\n", to.c_str());
	fprintf(sendmail, "Subject: %s\n", subject.c_str());
	if (headers.size() > 0)
	{
		fprintf(sendmail, "%s\n", headers.c_str());
	}
	fprintf(sendmail, "\n%s\n", message.c_str());

	if (pclose(sendmail) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}
}

//-------------------------------------------------------------------

void
network::mail(const dodoString &host,
			  int              port,
			  const dodoString &to,
			  const dodoString &from,
			  const dodoString &subject,
			  const dodoString &message,
			  const dodoString &login,
			  const dodoString &pass,
			  const dodoString &headers)
{
	using namespace io::network;

	enum mailAuthTypeEnum
	{
		SMTPAUTH_CRAMMD5 = 2,
		SMTPAUTH_LOGIN = 4,
		SMTPAUTH_PLAIN = 8
	};

	unsigned short authType = 0;

	bool auth = login.size() > 0 ? true : false;

	short family = CONNECTION_PROTO_FAMILY_IPV4;
	if (host.find(":") != dodoString::npos)
	{
		family = CONNECTION_PROTO_FAMILY_IPV6;
	}

	exchange ex;
	client net(family, CONNECTION_TRANSFER_TYPE_STREAM);

	net.connect(host, port, ex);

	dodoString mess;

	mess = ex.readStream();
	ex.writeStream("EHLO " + network::getLocalName() + "\r\n");
	mess = ex.readStream();

	if (string::stringToI(dodoString(mess.data(), 3)) != 250)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_LIBDODO, NETWORKEX_BADMAILHELO, TOOLSNETWORKEX_BADMAILHELO_STR, __LINE__, __FILE__);
	}

	if (auth)
	{
		if (string::contains(mess, "CRAM-MD5"))
		{
			addFlag(authType, SMTPAUTH_CRAMMD5);
		}

		if (string::contains(mess, "LOGIN"))
		{
			addFlag(authType, SMTPAUTH_LOGIN);
		}

		if (string::contains(mess, "PLAIN"))
		{
			addFlag(authType, SMTPAUTH_PLAIN);
		}
	}

	if (auth)
	{
		if (isSetFlag(authType, SMTPAUTH_CRAMMD5))
		{
			ex.writeStream("AUTH CRAM-MD5\r\n");
			mess = ex.readStream();

			if (string::stringToI(dodoString(mess.data(), 3)) != 334)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
			}

			dodoString ticket = code::decodeBase64(dodoString(mess.data() + 4, mess.size() - 4));

			dodoString md5pass;
			if (pass.size() > 64)
			{
				md5pass = code::MD5(pass);
			}
			else
			{
				md5pass = pass;
			}

			unsigned char ipad[65];
			unsigned char opad[65];

			memset(ipad, 0, 65);
			memset(opad, 0, 65);

			memcpy(ipad, md5pass.c_str(), md5pass.size());
			memcpy(opad, md5pass.c_str(), md5pass.size());

			for (short i = 0; i < 64; ++i)
			{
				ipad[i] ^= 0x36;
				opad[i] ^= 0x5c;
			}

			code::__MD5Context__ context;
			unsigned char digest[16];

			code::MD5Init(&context);
			code::MD5Update(&context, ipad, 64);
			code::MD5Update(&context, (unsigned char *)ticket.c_str(), ticket.size());
			code::MD5Final(digest, &context);

			code::MD5Init(&context);
			code::MD5Update(&context, opad, 64);
			code::MD5Update(&context, digest, 16);
			code::MD5Final(digest, &context);

			md5pass = code::binToHex(dodoString((char *)&digest, 16));

			ex.writeStream(code::encodeBase64(login + " " + md5pass) + "\r\n");
			mess = ex.readStream();

			if (string::stringToI(dodoString(mess.data(), 3)) != 235)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
			}
		}
		else
		{
			if (isSetFlag(authType, SMTPAUTH_LOGIN))
			{
				ex.writeStream("AUTH LOGIN\r\n");
				mess = ex.readStream();

				if (string::stringToI(dodoString(mess.data(), 3)) != 334)
				{
					throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
				}

				ex.writeStream(code::encodeBase64(login) + "\r\n");
				mess = ex.readStream();

				if (string::stringToI(dodoString(mess.data(), 3)) != 334)
				{
					throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
				}

				ex.writeStream(code::encodeBase64(pass) + "\r\n");
				mess = ex.readStream();

				if (string::stringToI(dodoString(mess.data(), 3)) != 235)
				{
					throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
				}
			}
			else
			{
				if (isSetFlag(authType, SMTPAUTH_PLAIN))
				{
					ex.writeStream("AUTH PLAIN" + code::encodeBase64(login + "\0" + login + "\0" + pass) + "\r\n");
					mess = ex.readStream();

					if (string::stringToI(dodoString(mess.data(), 3)) != 334)
					{
						throw exception::basic(exception::ERRMODULE_TOOLSNETWORK, NETWORKEX_MAIL, exception::ERRNO_ERRNO, NETWORKEX_BADMAILAUTH, TOOLSNETWORKEX_BADMAILAUTH_STR, __LINE__, __FILE__);
					}
				}
			}
		}
	}

	ex.writeStream("MAIL FROM: <" + from + ">\r\n");
	mess = ex.readStream();

	dodoStringArray pock = misc::split(to, ",");

	dodoStringArray::iterator i = pock.begin(), j = pock.end();
	for (; i != j; ++i)
	{
		ex.writeStream("RCPT TO: <" + *i + ">\r\n");
		mess = ex.readStream();
	}

	ex.writeStream("DATA\r\n");
	mess = ex.readStream();

	ex.writeStream("To: " + to + "\r\n");
	ex.writeStream("From: " + from + "\r\n");
	ex.writeStream("X-Mailer: " PACKAGE_NAME "/" PACKAGE_VERSION "\r\n");
	ex.writeStream("Subject: " + subject  + "\r\n");
	ex.writeStream(headers);
	ex.writeStream(message);
	ex.writeStream("\r\n.\r\n");
	ex.writeStream("QUIT\r\n");
}

//-------------------------------------------------------------------

