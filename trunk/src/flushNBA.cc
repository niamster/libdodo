/***************************************************************************
 *            flushNBA.cc
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

#include <libdodo/flushNBA.h>

using namespace dodo;

flushNBA::flushNBA(flushNBA &rt)
{
}

//-------------------------------------------------------------------

flushNBA::flushNBA() : descs(0)
{
}

//-------------------------------------------------------------------

flushNBA::~flushNBA()
{
}

//-------------------------------------------------------------------

int
flushNBA::addFlush(const flush &fl)
{
	__inOutDescriptors tempD;

	tempD.position = ++descs;
	tempD.in = fl.getInDescriptor();
	tempD.out = fl.getOutDescriptor();

	desc.push_back(tempD);

	return tempD.position;
}

//-------------------------------------------------------------------

void
flushNBA::makeFalse(int count) const
{
	for (int i = 0; i < count; ++i)
		tempRB.push_back(false);
}

//-------------------------------------------------------------------

dodoArray<bool>
flushNBA::isReadable(const dodoArray<int> &pos,
					 int timeout) const
{
	tempRB.clear();

	int count = -1;

	pollfd *fds = new pollfd[pos.size()];

	dodoArray<__inOutDescriptors>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i)
	{
		dodoArray<int>::const_iterator m(pos.begin()), n(pos.end());
		for (; m != n; ++m)
		{
			if (i->position == *m)
			{
				++count;

				fds[count].fd = i->in;
				fds[count].events = POLLIN | POLLPRI;
			}
		}
	}

	++count;

	if (count > 0)
	{
		int res = poll(fds, count, timeout);

		if (res > 0)
		{
			for (int i = 0; i < count; ++i)
			{
				if (isSetFlag(fds[i].revents, POLLIN) || isSetFlag(fds[i].revents, POLLPRI))
					tempRB.push_back(true);
				else
					tempRB.push_back(false);
			}

			delete [] fds;

			return tempRB;
		}
		else
		{
			if (res == 0)
			{
				makeFalse(count);

				delete [] fds;

				return tempRB;
			}
			else
			{
				delete [] fds;

				throw baseEx(ERRMODULE_FLUSHNBA, FLUSHNBA_ISREADABLE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}
	}

	delete [] fds;

	makeFalse(count);

	return tempRB;
}

//-------------------------------------------------------------------

dodoArray<bool>
flushNBA::isWritable(const dodoArray<int> &pos,
					 int timeout) const
{
	tempRB.clear();

	int count = -1;

	pollfd *fds = new pollfd[pos.size()];

	dodoArray<__inOutDescriptors>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i)
	{
		dodoArray<int>::const_iterator m(pos.begin()), n(pos.end());
		for (; m != n; ++m)
		{
			if (i->position == *m)
			{
				++count;

				fds[count].fd = i->out;
				fds[count].events = POLLOUT;
			}
		}
	}

	++count;

	if (count > 0)
	{
		int res = poll(fds, count, timeout);

		if (res > 0)
		{
			for (int i = 0; i < count; ++i)
			{
				if (isSetFlag(fds[i].revents, POLLOUT))
					tempRB.push_back(true);
				else
					tempRB.push_back(false);
			}

			delete [] fds;

			return tempRB;
		}
		else
		{
			if (res == 0)
			{
				delete [] fds;

				makeFalse(count);

				return tempRB;
			}
			else
			{
				delete [] fds;
				
				throw baseEx(ERRMODULE_FLUSHNBA, FLUSHNBA_ISWRITABLE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}
	}

	delete [] fds;

	makeFalse(count);

	return tempRB;
}

//-------------------------------------------------------------------

bool
flushNBA::isReadable(int pos,
					 int timeout) const
{
	pollfd fd;

	dodoArray<__inOutDescriptors>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i)
		if (i->position == pos)
		{
			fd.fd = i->in;
			fd.events = POLLIN | POLLPRI;

			int res = poll(&fd, 1, timeout);

			if (res > 0)
			{
				if (isSetFlag(fd.revents, POLLIN) || isSetFlag(fd.revents, POLLPRI))
					return true;
				else
					return false;
			}
			else
			{
				if (res == 0)
					return false;
				else
					throw baseEx(ERRMODULE_FLUSHNBA, FLUSHNBA_ISREADABLE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}

	return false;
}

//-------------------------------------------------------------------

void
flushNBA::delFlush(int pos)
{
	dodoArray<__inOutDescriptors>::iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i)
		if (i->position == pos)
		{
			desc.erase(i);

			break;
		}
}

//-------------------------------------------------------------------

bool
flushNBA::isWritable(int pos,
					 int timeout) const
{
	pollfd fd;

	dodoArray<__inOutDescriptors>::const_iterator i(desc.begin()), j(desc.end());
	for (; i != j; ++i)
		if (i->position == pos)
		{
			fd.fd = i->out;
			fd.events = POLLOUT;

			int res = poll(&fd, 1, timeout);

			if (res > 0)
			{
				if (isSetFlag(fd.revents, POLLOUT))
					return true;
				else
					return false;
			}
			else
			{
				if (res == 0)
					return false;
				else
					throw baseEx(ERRMODULE_FLUSHNBA, FLUSHNBA_ISWRITABLE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__);
			}
		}

	return false;
}

//-------------------------------------------------------------------
