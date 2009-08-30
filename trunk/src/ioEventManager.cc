/***************************************************************************
 *            ioEventManager.cc
 *
 *  Thu Sep 09 03:21:24 2006
 *  Copyright  2006  Ni@m
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

#include <poll.h>
#include <errno.h>
#include <string.h>

#include <libdodo/ioEventManager.h>
#include <libdodo/ioEventManagerEx.h>
#include <libdodo/types.h>
#include <libdodo/toolsMisc.h>
#include <libdodo/ioEventInfo.h>
#include <libdodo/pcSyncProcessSection.h>
#include <libdodo/pcSyncProtector.h>

using namespace dodo::io::event;

manager::manager(manager &rt)
{
}

//-------------------------------------------------------------------

manager::manager() : descs(0),
				 keeper(new pc::sync::process::section(0))
{
}

//-------------------------------------------------------------------

manager::~manager()
{
	delete keeper;
}

//-------------------------------------------------------------------

int
manager::add(const info &fl)
{
	pc::sync::protector pg(keeper);

	__descriptors__ tempD;

	tempD.position = ++descs;
	tempD.in = fl.inDescriptor();
	tempD.out = fl.outDescriptor();

	desc.push_back(tempD);

	return tempD.position;
}

//-------------------------------------------------------------------

dodoArray<bool>
manager::isReadable(const dodoArray<int> &pos,
				  int                  timeout) const
{
	pc::sync::protector pg(keeper);

	int count = -1;

	pollfd *fds = new pollfd[pos.size()];

	dodoArray<__descriptors__>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i) {
		dodoArray<int>::const_iterator m(pos.begin()), n(pos.end());
		for (; m != n; ++m) {
			if (i->position == *m) {
				++count;

				fds[count].fd = i->in;
				fds[count].events = POLLIN | POLLPRI;
			}
		}
	}

	++count;

	dodoArray<bool> tempRB;

	if (count > 0) {
		int res = poll(fds, count, timeout);

		if (res > 0) {
			for (int i = 0; i < count; ++i) {
				if (isSetFlag(fds[i].revents, POLLIN) || isSetFlag(fds[i].revents, POLLPRI))
					tempRB.push_back(true);
				else
					tempRB.push_back(false);
			}

			delete [] fds;

			return tempRB;
		} else {
			if (res == 0) {
				for (int i = 0; i < count; ++i)
					tempRB.push_back(false);

				delete [] fds;

				return tempRB;
			} else {
				delete [] fds;

				throw exception::basic(exception::MODULE_IOEVENTMANAGER, MANAGEREX_ISREADABLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}
	}

	delete [] fds;

	for (int i = 0; i < count; ++i)
		tempRB.push_back(false);

	return tempRB;
}

//-------------------------------------------------------------------

dodoArray<bool>
manager::isWritable(const dodoArray<int> &pos,
				  int                  timeout) const
{
	pc::sync::protector pg(keeper);

	int count = -1;

	pollfd *fds = new pollfd[pos.size()];

	dodoArray<__descriptors__>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i) {
		dodoArray<int>::const_iterator m(pos.begin()), n(pos.end());
		for (; m != n; ++m) {
			if (i->position == *m) {
				++count;

				fds[count].fd = i->out;
				fds[count].events = POLLOUT;
			}
		}
	}

	++count;

	dodoArray<bool> tempRB;

	if (count > 0) {
		int res = poll(fds, count, timeout);

		if (res > 0) {
			for (int i = 0; i < count; ++i) {
				if (isSetFlag(fds[i].revents, POLLOUT))
					tempRB.push_back(true);
				else
					tempRB.push_back(false);
			}

			delete [] fds;

			return tempRB;
		} else {
			if (res == 0) {
				delete [] fds;

				for (int i = 0; i < count; ++i)
					tempRB.push_back(false);

				return tempRB;
			} else {
				delete [] fds;

				throw exception::basic(exception::MODULE_IOEVENTMANAGER, MANAGEREX_ISWRITABLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}
	}

	delete [] fds;

	for (int i = 0; i < count; ++i)
		tempRB.push_back(false);

	return tempRB;
}

//-------------------------------------------------------------------

bool
manager::isReadable(int pos,
				  int timeout) const
{
	pc::sync::protector pg(keeper);

	pollfd fd;

	dodoArray<__descriptors__>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i) {
		if (i->position == pos) {
			fd.fd = i->in;
			fd.events = POLLIN | POLLPRI;

			int res = poll(&fd, 1, timeout);

			if (res > 0) {
				if (isSetFlag(fd.revents, POLLIN) || isSetFlag(fd.revents, POLLPRI))
					return true;
				else
					return false;
			} else {
				if (res == 0)
					return false;
				else
					throw exception::basic(exception::MODULE_IOEVENTMANAGER, MANAGEREX_ISREADABLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------

void
manager::remove(int pos)
{
	pc::sync::protector pg(keeper);

	dodoArray<__descriptors__>::iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i) {
		if (i->position == pos) {
			desc.erase(i);

			break;
		}
	}
}

//-------------------------------------------------------------------

bool
manager::isWritable(int pos,
				  int timeout) const
{
	pc::sync::protector pg(keeper);

	pollfd fd;

	dodoArray<__descriptors__>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i) {
		if (i->position == pos) {
			fd.fd = i->out;
			fd.events = POLLOUT;

			int res = poll(&fd, 1, timeout);

			if (res > 0) {
				if (isSetFlag(fd.revents, POLLOUT))
					return true;
				else
					return false;
			} else {
				if (res == 0)
					return false;
				else
					throw exception::basic(exception::MODULE_IOEVENTMANAGER, MANAGEREX_ISWRITABLE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------

