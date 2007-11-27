/***************************************************************************
 *            flushDiskTools.cc
 *
 *  Wed Oct 8 01:44:18 2005
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

#include <libdodo/flushDiskTools.h>

using namespace dodo;

void
flushDiskTools::unlink(const dodoString &path,
					   bool force)
{
	int status(0);
	struct stat st;

	if (::lstat(path.c_str(), &st) == -1)
		if (errno != ENOENT || !force)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (S_ISDIR(st.st_mode))
		status = ::rmdir(path.c_str());
	else
		status = ::unlink(path.c_str());

	if (status == -1)
		if (errno != ENOENT || !force)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
flushDiskTools::rename(const dodoString &oldPath,
					   const dodoString &newPath)
{
	if (::rename(oldPath.c_str(), newPath.c_str()) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_RENAME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
flushDiskTools::symlink(const dodoString &oldPath,
						const dodoString &newPath,
						bool force)
{
	if (force)
	{
		struct stat st;
		if (::lstat(newPath.c_str(), &st) != -1)

			if (!S_ISLNK(st.st_mode))
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_SYMLINK, ERR_LIBDODO, FLUSHDISKTOOLS_WRONG_FILENAME, FLUSHDISKTOOLS_WRONG_FILENAME_STR, __LINE__, __FILE__, newPath);
			else
			if (::unlink(newPath.c_str()) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_SYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, newPath);
	}

	if (::symlink(oldPath.c_str(), newPath.c_str()) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_SYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
flushDiskTools::link(const dodoString &oldPath,
					 const dodoString &newPath)
{
	if  (::link(oldPath.c_str(), newPath.c_str()) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_LINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
flushDiskTools::chown(const dodoString &path,
					  int uid)
{
	if (::chown(path.c_str(), uid, (unsigned int)-1) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_CHOWN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
flushDiskTools::chgrp(const dodoString &path,
					  int gid)
{
	if (::chown(path.c_str(), (unsigned int)-1, gid) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_CHGRP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

int
flushDiskTools::getUserOwner(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETUSEROWNER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_uid;
}

//-------------------------------------------------------------------

int
flushDiskTools::getGroupOwner(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETGROUPOWNER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_gid;
}

//-------------------------------------------------------------------

void
flushDiskTools::touch(const dodoString &path,
					  int a_time)
{
	if (a_time == 1)
		a_time = time(NULL);

	utimbuf temp = { a_time, a_time };

	if (::utime(path.c_str(), &temp) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_TOUCH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
flushDiskTools::mkdir(const dodoString &path,
					  int permissions,
					  bool force)
{
	if (::mkdir(path.c_str(), getPermission(permissions)) == -1)
	{
		if (force && (errno == EEXIST))
		{
			struct stat st;
			if (::lstat(path.c_str(), &st) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_MKDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

			if (S_ISDIR(st.st_mode))
				return ;
		}
		else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_MKDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

void
flushDiskTools::chmod(const dodoString &path, int permissions)
{
	if (::chmod(path.c_str(), getPermission(permissions)) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_CHMOD, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

int
flushDiskTools::getPermission(int permission)
{
	int mode(0);

	if (isSetFlag(permission, PERM_OWNER_READ_ACCESS))
		mode |= S_IRUSR;

	if (isSetFlag(permission, PERM_GROUP_READ_ACCESS))
		mode |= S_IRGRP;
	if (isSetFlag(permission, PERM_OTHER_READ_ACCESS))
		mode |= S_IROTH;

	if (isSetFlag(permission, PERM_OWNER_WRITE_ACCESS))
		mode |= S_IWUSR;

	if (isSetFlag(permission, PERM_GROUP_WRITE_ACCESS))
		mode |= S_IWGRP;
	if (isSetFlag(permission, PERM_OTHER_WRITE_ACCESS))
		mode |= S_IWOTH;

	if (isSetFlag(permission, PERM_STICKY_ACCESS))
		mode |= S_ISVTX;

	if (isSetFlag(permission, PERM_OWNER_EXECUTE_ACCESS))
		mode |= S_IXUSR;
	if (isSetFlag(permission, PERM_GROUP_EXECUTE_ACCESS))
		mode |= S_IXGRP;
	if (isSetFlag(permission, PERM_OTHER_EXECUTE_ACCESS))
		mode |= S_IXOTH;

	if (isSetFlag(permission, PERM_SUID_ACCESS))
		mode |= S_ISUID;
	if (isSetFlag(permission, PERM_SGID_ACCESS))
		mode |= S_ISGID;

	return mode;

}

//-------------------------------------------------------------------

void
flushDiskTools::rm(const dodoString &path,
				   bool force)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		if (errno != ENOENT || !force)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		else
			return ;

	if (!S_ISDIR(st.st_mode))
	{
		if (::unlink(path.c_str()) == -1)
			if (errno != ENOENT || !force)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
	else
	{
		dodoString attached;

		DIR *directory = opendir(path.c_str());

		if (directory == NULL)
		{
			if (errno != ENOENT || !force)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
		{
			dirent *dd;

			while ((dd = readdir(directory)) != NULL)
			{
				if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
					continue;

				attached.assign(path + FILE_DELIM + dd->d_name);

				if (::lstat(attached.c_str(), &st) == -1)
					if (errno != ENOENT || !force)
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

				if (S_ISDIR(st.st_mode))
					flushDiskTools::rm(attached.c_str());
				else
				if (::unlink(attached.c_str()) == -1)
					if (errno != ENOENT || !force)
						throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

			closedir(directory);

			if (::rmdir(path.c_str()) == -1)
				if (errno != ENOENT || !force)
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
	}
}

//-------------------------------------------------------------------

int
flushDiskTools::getPermissions(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETPERMISSIONS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	int mode(PERM_NONE);

	if (isSetFlag(st.st_mode, S_IRUSR))
		mode |= PERM_OWNER_READ_ACCESS;

	if (isSetFlag(st.st_mode, S_IRGRP))
		mode |= PERM_GROUP_READ_ACCESS;
	if (isSetFlag(st.st_mode, S_IROTH))
		mode |= PERM_OTHER_READ_ACCESS;

	if (isSetFlag(st.st_mode, S_IWUSR))
		mode |= PERM_OWNER_WRITE_ACCESS;

	if (isSetFlag(st.st_mode, S_IWGRP))
		mode |= PERM_GROUP_WRITE_ACCESS;
	if (isSetFlag(st.st_mode, S_IWOTH))
		mode |= PERM_OTHER_WRITE_ACCESS;

	if (isSetFlag(st.st_mode, S_ISVTX))
		mode |= PERM_STICKY_ACCESS;

	if (isSetFlag(st.st_mode, S_IXUSR))
		mode |= PERM_OWNER_EXECUTE_ACCESS;
	if (isSetFlag(st.st_mode, S_IXGRP))
		mode |= PERM_GROUP_EXECUTE_ACCESS;
	if (isSetFlag(st.st_mode, S_IXOTH))
		mode |= PERM_OTHER_EXECUTE_ACCESS;

	if (isSetFlag(st.st_mode, S_ISUID))
		mode |= PERM_SUID_ACCESS;
	if (isSetFlag(st.st_mode, S_ISGID))
		mode |= PERM_SGID_ACCESS;

	return mode;
}

//-------------------------------------------------------------------

int
flushDiskTools::getFileType(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILETYPE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	st.st_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);

	switch (st.st_mode)
	{
		case S_IFREG:

			return FILETYPE_REGULAR_FILE;

		case S_IFDIR:

			return FILETYPE_DIRECTORY;

		case S_IFLNK:

			return FILETYPE_SYMBOLIC_LINK;

		case S_IFSOCK:

			return FILETYPE_LOCAL_SOCKET;

		case S_IFBLK:

			return FILETYPE_BLOCK_DEVICE;

		case S_IFCHR:

			return FILETYPE_CHARACTER_DEVICE;

		case S_IFIFO:

			return FILETYPE_FIFO;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

long
flushDiskTools::getSize(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETSIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_size;
}

//-------------------------------------------------------------------

long
flushDiskTools::getAccTime(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETACCTIME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_atime;
}

//-------------------------------------------------------------------

long
flushDiskTools::getModTime(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETMODTIME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_mtime;
}

//-------------------------------------------------------------------

__fileInfo
flushDiskTools::getFileInfo(const dodoString &path)
{
	__fileInfo file;

	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	file.name.assign(::basename((char *)path.c_str()));
	file.type = flushDiskTools::getFileType(path);
	file.perm = flushDiskTools::getPermissions(path);
	file.accTime = flushDiskTools::getAccTime(path);
	file.modTime = flushDiskTools::getModTime(path);
	file.size = flushDiskTools::getSize(path);
	file.uid = flushDiskTools::getUserOwner(path);
	file.gid = flushDiskTools::getGroupOwner(path);

	return file;
}

//-------------------------------------------------------------------

dodoArray<__fileInfo>
flushDiskTools::getDirInfo(const dodoString &path)
{
	dodoArray<__fileInfo> dir;
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETDIRINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!S_ISDIR(st.st_mode))
		return dir;


	DIR *directory = opendir(path.c_str());

	if (directory == NULL)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETDIRINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	dirent *dd;
	dodoString attached;

	while ((dd = readdir(directory)) != NULL)
	{
		if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
			continue;
		attached.assign(path + FILE_DELIM + dd->d_name);
		dir.push_back(flushDiskTools::getFileInfo(attached));
	}

	return dir;
}

//-------------------------------------------------------------------

void
flushDiskTools::followSymlink(const dodoString &path,
							  dodoString &original)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_FOLLOWSYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	char buffer[MAXPATHLEN];

	if (!S_ISLNK(st.st_mode))
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_SYMLINK, ERR_LIBDODO, FLUSHDISKTOOLS_WRONG_FILENAME, FLUSHDISKTOOLS_WRONG_FILENAME_STR, __LINE__, __FILE__, path);

	int count = 0;

	if ((count = ::readlink(path.c_str(), buffer, MAXPATHLEN)) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_FOLLOWSYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	original.assign(buffer, count);
}

//-------------------------------------------------------------------

dodoString
flushDiskTools::getFileContent(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!S_ISREG(st.st_mode))
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENT, ERR_LIBDODO, FLUSHDISKTOOLS_WRONG_FILENAME, FLUSHDISKTOOLS_WRONG_FILENAME_STR, __LINE__, __FILE__, path);

	FILE *file = fopen(path.c_str(), "r");
	if (file == NULL)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	char buffer[INSIZE];

	long iter = st.st_size / INSIZE, rest = st.st_size % INSIZE;
	dodoString retS = "";

	int i(0);
	for (; i < iter; ++i)
	{
		if (fseek(file, i * INSIZE, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		if (fread(buffer, INSIZE, 1, file) == 0)
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHDISKTOOLS_GETFILECONTENT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

		retS.append(buffer, INSIZE);
	}
	if (rest > 0)
	{
		if (fseek(file, i * INSIZE, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		if (fread(buffer, rest, 1, file) == 0)
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					throw baseEx(ERRMODULE_FLUSHSTD, FLUSHDISKTOOLS_GETFILECONTENT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

		retS.append(buffer, rest);
	}

	if (fclose(file) != 0)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENT, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return retS;
}

//-------------------------------------------------------------------

dodoStringArr
flushDiskTools::getFileContentArr(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENTARR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!S_ISREG(st.st_mode))
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENTARR, ERR_LIBDODO, FLUSHDISKTOOLS_WRONG_FILENAME, FLUSHDISKTOOLS_WRONG_FILENAME_STR, __LINE__, __FILE__, path);

	FILE *file = fopen(path.c_str(), "r");
	if (file == NULL)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENTARR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	char buffer[DISK_MAXLINELEN];
	dodoStringArr arr;

	while (fgets(buffer, DISK_MAXLINELEN, file) != NULL)
		arr.push_back(buffer);

	if (fclose(file) != 0)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_GETFILECONTENTARR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return arr;

}

//-------------------------------------------------------------------

dodoString
flushDiskTools::lastname(const dodoString &path)
{
	char tempB[MAXPATHLEN];

	strcpy(tempB, path.c_str());

	::basename((char *)path.c_str());

	dodoString result(tempB);

	return result;
}

//-------------------------------------------------------------------

dodoString
flushDiskTools::dirname(const dodoString &path)
{
	char tempB[MAXPATHLEN];

	strcpy(tempB, path.c_str());

	::dirname((char *)path.c_str());

	dodoString result(tempB);

	return result;
}

//-------------------------------------------------------------------

void
flushDiskTools::copy(const dodoString &from,
					 const dodoString &a_to,
					 bool force)
{
	dodoString to = a_to;

	{
		char tempB[MAXPATHLEN];

		strcpy(tempB, to.c_str());
		char *toT = ::basename(tempB);
		if (strcmp(toT, "..") == 0  || strcmp(toT, ".") == 0 || a_to[a_to.size() - 1] == FILE_DELIM)
			to = toT + dodoString(1, FILE_DELIM) + ::basename((char *)from.c_str());
	}

	struct stat stFrom, stTo;

	if (::lstat(from.c_str(), &stFrom) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

	if (::lstat(to.c_str(), &stTo) == -1)
	{
		if (errno != ENOENT)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
	}
	else
	{
		if (force)
		{
			if (!S_ISDIR(stTo.st_mode))
			{
				if (::unlink(to.c_str()) == -1)
					throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
			}
			else
			if (::rmdir(to.c_str()) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

		}
		else
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_LIBDODO, FLUSHDISKTOOLS_WRONG_FILENAME, FLUSHDISKTOOLS_WRONG_FILENAME_STR, __LINE__, __FILE__, to);
	}

	if (!S_ISREG(stFrom.st_mode))
	{
		if (S_ISDIR(stFrom.st_mode))
		{
			if (::mkdir(to.c_str(), stFrom.st_mode) == 1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
		}
		else
		if (S_ISLNK(stFrom.st_mode))
		{
			char buffer[MAXPATHLEN];
			int count = 0;

			if ((count = ::readlink(from.c_str(), buffer, MAXPATHLEN)) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

			buffer[count] = '\0';

			if (::symlink(buffer, to.c_str()) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, dodoString(buffer) + "->" + to);
		}
		else
		if (::mknod(to.c_str(), stFrom.st_mode, 0) == 1)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
	}
	else
	{
		long iter = stFrom.st_size / INSIZE, rest = stFrom.st_size % INSIZE;

		FILE *fromFile = fopen(from.c_str(), "r");
		if (fromFile == NULL)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

		FILE *toFile = fopen(to.c_str(), "w+");
		if (toFile == NULL)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

		char buffer[INSIZE];

		int i(0), j;
		for (; i < iter; ++i)
		{
			j = i * INSIZE;

			if (fseek(fromFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fseek(toFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fread(buffer, INSIZE, 1, fromFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_FLUSHSTD, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}

			if (fwrite(buffer, INSIZE, 1, toFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_FLUSHSTD, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
		}
		if (rest > 0)
		{
			j = i * INSIZE;
			if (fseek(fromFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fseek(toFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fread(buffer, rest, 1, fromFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_FLUSHSTD, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}

			if (fwrite(buffer, rest, 1, toFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_FLUSHSTD, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
		}

		if (fclose(fromFile) != 0)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

		if (fclose(toFile) != 0)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
	}
}

//-------------------------------------------------------------------

void
flushDiskTools::copyDir(const dodoString &from,
						const dodoString &a_to,
						bool force)
{
	dodoString to = a_to;

	{
		char tempB[MAXPATHLEN];

		strcpy(tempB, to.c_str());
		char *toT = ::basename(tempB);
		if (strcmp(toT, "..") == 0 || strcmp(toT, ".") == 0 || a_to[a_to.size() - 1] == FILE_DELIM)
			to = toT + dodoString(1, FILE_DELIM) + ::basename((char *)from.c_str());
	}

	struct stat stFrom, stTo;

	if (::lstat(from.c_str(), &stFrom) == -1)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

	if (::lstat(to.c_str(), &stTo) == -1)
	{
		if (errno != ENOENT)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
	}
	else
	if (force)
		flushDiskTools::rm(to, force);
	else
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPYDIR, ERR_LIBDODO, FLUSHDISKTOOLS_WRONG_FILENAME, FLUSHDISKTOOLS_WRONG_FILENAME_STR, __LINE__, __FILE__, to);

	if (!S_ISDIR(stFrom.st_mode))
		flushDiskTools::copy(from, to, force);
	else
	{
		if (::mkdir(to.c_str(), stFrom.st_mode) == -1)
			throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

		dodoString attachedFrom, attachedTo;

		DIR *directory = opendir(from.c_str());
		if (directory == NULL)
		{
			if (errno == ENOENT)
				return ;
			else
				throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
		}

		dirent *dd;

		while ((dd = readdir(directory)) != NULL)
		{
			if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
				continue;

			attachedTo.assign(to + FILE_DELIM + dd->d_name);
			attachedFrom.assign(from + FILE_DELIM + dd->d_name);

			flushDiskTools::copyDir(attachedFrom, attachedTo, force);
		}

		closedir(directory);
	}
}

//-------------------------------------------------------------------

bool
flushDiskTools::exists(const dodoString &path)
{
	struct stat st;

	if (::lstat(path.c_str(), &st) == 0)
		return true;

	return false;
}

//-------------------------------------------------------------------

void
flushDiskTools::append(const dodoString &path,
					   const dodoString &content)
{
	FILE *file = fopen(path.c_str(), "a+");
	if (file == NULL)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_APPEND, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (fwrite(content.c_str(), content.size(), 1, file) == 0)
		throw baseEx(ERRMODULE_FLUSHDISKTOOLS, FLUSHDISKTOOLS_APPEND, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------
