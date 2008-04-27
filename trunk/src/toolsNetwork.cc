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

#include <libdodo/toolsNetwork.h>

using namespace dodo;

__hostInfo
toolsNetwork::getHostInfo(const dodoString &host)
{
	hostent *ent = gethostbyname(host.c_str());

	if (ent == NULL)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETHOSTINFO, ERR_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

	__hostInfo info;
	info.name = ent->h_name;

	int i(0);

	while (ent->h_aliases[i] != NULL)
		info.aliases.push_back(ent->h_aliases[i++]);

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
toolsNetwork::getHostPrimaryIp(const dodoString &host)
{
	hostent *ent = gethostbyname(host.c_str());

	if (ent == NULL)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETHOSTPRIMARYIP, ERR_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

	char temp[INET6_ADDRSTRLEN];

	if (ent->h_addr_list[0] != NULL)
	{
		switch (ent->h_addrtype)
		{
			case AF_INET:

				if (inet_ntop(AF_INET, ent->h_addr_list[0], temp, INET_ADDRSTRLEN) == NULL)
					throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETHOSTPRIMARYIP, ERR_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

				break;

			case AF_INET6:

				if (inet_ntop(AF_INET6, ent->h_addr_list[0], temp, INET6_ADDRSTRLEN) == NULL)
					throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETHOSTPRIMARYIP, ERR_H_ERRNO, h_errno, hstrerror(h_errno), __LINE__, __FILE__);

				break;
		}
	}

	return temp;
}

//-------------------------------------------------------------------

dodoStringArray
toolsNetwork::getInterfacesNames()
{
	struct if_nameindex *ifaces = if_nameindex();
	if (ifaces == NULL)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACESNAMES, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	int i(-1);
	dodoStringArray arr;

	while (ifaces[++i].if_index != 0)
		arr.push_back(ifaces[i].if_name);

	if_freenameindex(ifaces);

	return arr;

}

//-------------------------------------------------------------------

__servInfo
toolsNetwork::getServiceInfo(const dodoString &host,
							  const dodoString &protocol)
{
	servent *ent = getservbyname(host.c_str(), protocol.c_str());

	__servInfo info;

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

__servInfo
toolsNetwork::getServiceInfo(int port,
							  const dodoString &protocol)
{
	servent *ent = getservbyport(port, protocol.c_str());

	__servInfo info;

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

__ifInfo
toolsNetwork::getInterfaceInfo(const dodoString &interface)
{
	int socket = ::socket(PF_INET, SOCK_DGRAM, 0);
	if (socket == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	ifreq ifr;
	strcpy(ifr.ifr_name, interface.c_str());

	__ifInfo info;
	char add[INET6_ADDRSTRLEN];

	sockaddr_in sin;

	if (::ioctl(socket, SIOCGIFADDR, &ifr) == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	memcpy((void *)&sin, &ifr.ifr_ifru.ifru_addr, sizeof(sockaddr));

	if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
		info.address = add;

#ifdef __FreeBSD__


#else

	if (::ioctl(socket, SIOCGIFNETMASK, &ifr) == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	memcpy((void *)&sin, &ifr.ifr_ifru.ifru_netmask, sizeof(sockaddr));

	if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
		info.netmask = add;

#endif

	if (::ioctl(socket, SIOCGIFBRDADDR, &ifr) == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	memcpy((void *)&sin, &ifr.ifr_ifru.ifru_broadaddr, sizeof(sockaddr));

	if (inet_ntop(AF_INET, &sin.sin_addr, add, INET_ADDRSTRLEN) != NULL)
		info.broadcast = add;

#ifdef __FreeBSD__


#else

	if (::ioctl(socket, SIOCGIFHWADDR, &ifr) == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	sprintf(add, "%.2X:%.2X:%.2X:%.2X:%.2X:%.2X", ifr.ifr_ifru.ifru_hwaddr.sa_data[0] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[1] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[2] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[3] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[4] & 0xff,
			ifr.ifr_ifru.ifru_hwaddr.sa_data[5] & 0xff);

#endif

	info.hwaddr = add;

	if (::ioctl(socket, SIOCGIFFLAGS, &ifr) == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

	if (::close(socket) == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETINTERFACEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);

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

dodoString
toolsNetwork::getLocalName()
{
	dodoString temp0;
	char *temp1 = new char[256];

	if (::gethostname(temp1, 255) == -1)
	{
		delete [] temp1;

		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_GETLOCALNAME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
	}

	temp0.assign(temp1);

	delete [] temp1;

	return temp0;
}

//-------------------------------------------------------------------

void
toolsNetwork::setLocalName(const dodoString &host)
{
	if (::sethostname(host.c_str(), host.size()) == -1)
		throw baseEx(ERRMODULE_IONETWORKTOOLS, IONETWORKTOOLSEX_SETLOCALNAME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
}

//-------------------------------------------------------------------

