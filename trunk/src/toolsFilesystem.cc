/***************************************************************************
 *            toolsFilesystem.cc
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

#include <libdodo/toolsFilesystem.h>

using namespace dodo;

void
toolsFilesystem::unlink(const dodoString &path,
					bool force)
{
	int status(0);
	struct stat st;

	if (::lstat(path.c_str(), &st) == -1)
		if (errno != ENOENT || !force)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (S_ISDIR(st.st_mode))
		status = ::rmdir(path.c_str());
	else
		status = ::unlink(path.c_str());

	if (status == -1)
		if (errno != ENOENT || !force)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
toolsFilesystem::rename(const dodoString &oldPath,
					const dodoString &newPath)
{
	if (::rename(oldPath.c_str(), newPath.c_str()) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RENAME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
toolsFilesystem::symlink(const dodoString &oldPath,
					 const dodoString &newPath,
					 bool force)
{
	if (force)
	{
		struct stat st;
		if (::lstat(newPath.c_str(), &st) != -1)
		{
			if (!S_ISLNK(st.st_mode))
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_SYMLINK, ERR_LIBDODO, TOOLSFILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, newPath);
			else
			if (::unlink(newPath.c_str()) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_SYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, newPath);
		}
	}

	if (::symlink(oldPath.c_str(), newPath.c_str()) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_SYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
toolsFilesystem::link(const dodoString &oldPath,
				  const dodoString &newPath)
{
	if  (::link(oldPath.c_str(), newPath.c_str()) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_LINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
toolsFilesystem::chown(const dodoString &path,
				   int uid)
{
	if (::chown(path.c_str(), uid, (unsigned int)-1) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_CHOWN, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
toolsFilesystem::chgrp(const dodoString &path,
				   int gid)
{
	if (::chown(path.c_str(), (unsigned int)-1, gid) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_CHGRP, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

int
toolsFilesystem::getUserOwner(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETUSEROWNER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_uid;
}

//-------------------------------------------------------------------

int
toolsFilesystem::getGroupOwner(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETGROUPOWNER, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_gid;
}

//-------------------------------------------------------------------

void
toolsFilesystem::touch(const dodoString &path,
				   int a_time)
{
	if (a_time == 1)
		a_time = time(NULL);

	utimbuf temp = { a_time, a_time };

	if (::utime(path.c_str(), &temp) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_TOUCH, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
toolsFilesystem::mkfifo(const dodoString &path,
					int permissions)
{
	if (::mkfifo(path.c_str(), toRealPermission(permissions)) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKFIFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
toolsFilesystem::mkdir(const dodoString &path,
				   int permissions,
				   bool force)
{
	if (::mkdir(path.c_str(), toRealPermission(permissions)) == -1)
	{
		if (force && (errno == EEXIST))
		{
			struct stat st;
			if (::lstat(path.c_str(), &st) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

			if (!S_ISDIR(st.st_mode))
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKDIR, ERR_LIBDODO, TOOLSFILESYSTEMEX_NOTADIR, TOOLSFILESYSTEMEX_NOTADIR_STR, __LINE__, __FILE__, path);
		}
		else
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

void
toolsFilesystem::mkdirRecursive(const dodoString &path,
							int permissions)
{
	if (::mkdir(path.c_str(), toRealPermission(permissions)) == -1)
	{
		if (errno == EEXIST)
		{
			struct stat st;
			if (::lstat(path.c_str(), &st) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKDIRRECURSIVE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

			if (!S_ISDIR(st.st_mode))
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKDIRRECURSIVE, ERR_LIBDODO, TOOLSFILESYSTEMEX_NOTADIR, TOOLSFILESYSTEMEX_NOTADIR_STR, __LINE__, __FILE__, path);
		}
		else
		{
			mkdirRecursive(dirname(path), permissions);

			if (::mkdir(path.c_str(), toRealPermission(permissions)) == -1)
				if (errno == EEXIST)
				{
					struct stat st;
					if (::lstat(path.c_str(), &st) == -1)
						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKDIRRECURSIVE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

					if (!S_ISDIR(st.st_mode))
						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_MKDIRRECURSIVE, ERR_LIBDODO, TOOLSFILESYSTEMEX_NOTADIR, TOOLSFILESYSTEMEX_NOTADIR_STR, __LINE__, __FILE__, path);
				}
		}
	}
}

//-------------------------------------------------------------------

void
toolsFilesystem::chmod(const dodoString &path, int permissions)
{
	if (::chmod(path.c_str(), toRealPermission(permissions)) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_CHMOD, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

int
toolsFilesystem::toRealPermission(int permission)
{
	int mode(0);

	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_OWNER_READ_ACCESS))
		mode |= S_IRUSR;
	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_GROUP_READ_ACCESS))
		mode |= S_IRGRP;
	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_OTHER_READ_ACCESS))
		mode |= S_IROTH;

	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_OWNER_WRITE_ACCESS))
		mode |= S_IWUSR;
	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_GROUP_WRITE_ACCESS))
		mode |= S_IWGRP;
	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_OTHER_WRITE_ACCESS))
		mode |= S_IWOTH;

	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_OWNER_EXECUTE_ACCESS))
		mode |= S_IXUSR;
	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_GROUP_EXECUTE_ACCESS))
		mode |= S_IXGRP;
	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_OTHER_EXECUTE_ACCESS))
		mode |= S_IXOTH;

	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_STICKY_ACCESS))
		mode |= S_ISVTX;

	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_SUID_ACCESS))
		mode |= S_ISUID;
	if (isSetFlag(permission, TOOLSFILESYSTEM_PERM_SGID_ACCESS))
		mode |= S_ISGID;

	return mode;

}

//-------------------------------------------------------------------

void
toolsFilesystem::rm(const dodoString &path,
				bool force)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		if (errno != ENOENT || !force)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		else
			return ;

	if (!S_ISDIR(st.st_mode))
	{
		if (::unlink(path.c_str()) == -1)
			if (errno != ENOENT || !force)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
	else
	{
		dodoString attached;

		DIR *directory = opendir(path.c_str());

		if (directory == NULL)
		{
			if (errno != ENOENT || !force)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
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
						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

				if (S_ISDIR(st.st_mode))
					toolsFilesystem::rm(attached.c_str());
				else
				if (::unlink(attached.c_str()) == -1)
					if (errno != ENOENT || !force)
						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

			if (closedir(directory) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

			if (::rmdir(path.c_str()) == -1)
				if (errno != ENOENT || !force)
					throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_RM, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
	}
}

//-------------------------------------------------------------------

int
toolsFilesystem::getPermissions(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETPERMISSIONS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	int mode(TOOLSFILESYSTEM_PERM_NONE);

	if (isSetFlag(st.st_mode, S_IRUSR))
		mode |= TOOLSFILESYSTEM_PERM_OWNER_READ_ACCESS;
	if (isSetFlag(st.st_mode, S_IRGRP))
		mode |= TOOLSFILESYSTEM_PERM_GROUP_READ_ACCESS;
	if (isSetFlag(st.st_mode, S_IROTH))
		mode |= TOOLSFILESYSTEM_PERM_OTHER_READ_ACCESS;

	if (isSetFlag(st.st_mode, S_IWUSR))
		mode |= TOOLSFILESYSTEM_PERM_OWNER_WRITE_ACCESS;
	if (isSetFlag(st.st_mode, S_IWGRP))
		mode |= TOOLSFILESYSTEM_PERM_GROUP_WRITE_ACCESS;
	if (isSetFlag(st.st_mode, S_IWOTH))
		mode |= TOOLSFILESYSTEM_PERM_OTHER_WRITE_ACCESS;

	if (isSetFlag(st.st_mode, S_ISVTX))
		mode |= TOOLSFILESYSTEM_PERM_STICKY_ACCESS;

	if (isSetFlag(st.st_mode, S_IXUSR))
		mode |= TOOLSFILESYSTEM_PERM_OWNER_EXECUTE_ACCESS;
	if (isSetFlag(st.st_mode, S_IXGRP))
		mode |= TOOLSFILESYSTEM_PERM_GROUP_EXECUTE_ACCESS;
	if (isSetFlag(st.st_mode, S_IXOTH))
		mode |= TOOLSFILESYSTEM_PERM_OTHER_EXECUTE_ACCESS;

	if (isSetFlag(st.st_mode, S_ISUID))
		mode |= TOOLSFILESYSTEM_PERM_SUID_ACCESS;
	if (isSetFlag(st.st_mode, S_ISGID))
		mode |= TOOLSFILESYSTEM_PERM_SGID_ACCESS;

	return mode;
}

//-------------------------------------------------------------------

int
toolsFilesystem::getFileType(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILETYPE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	st.st_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);

	switch (st.st_mode)
	{
		case S_IFREG:

			return TOOLSFILESYSTEM_FILETYPE_REGULAR_FILE;

		case S_IFDIR:

			return TOOLSFILESYSTEM_FILETYPE_DIRECTORY;

		case S_IFLNK:

			return TOOLSFILESYSTEM_FILETYPE_SYMBOLIC_LINK;

		case S_IFSOCK:

			return TOOLSFILESYSTEM_FILETYPE_LOCAL_SOCKET;

		case S_IFBLK:

			return TOOLSFILESYSTEM_FILETYPE_BLOCK_DEVICE;

		case S_IFCHR:

			return TOOLSFILESYSTEM_FILETYPE_CHARACTER_DEVICE;

		case S_IFIFO:

			return TOOLSFILESYSTEM_FILETYPE_FIFO;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

long
toolsFilesystem::getSize(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETSIZE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_size;
}

//-------------------------------------------------------------------

long
toolsFilesystem::getAccTime(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETACCTIME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_atime;
}

//-------------------------------------------------------------------

long
toolsFilesystem::getModTime(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETMODTIME, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return st.st_mtime;
}

//-------------------------------------------------------------------

__fileInfo
toolsFilesystem::getFileInfo(const dodoString &path)
{
	__fileInfo file;

	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILEINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	file.name.assign(::basename((char *)path.c_str()));
	file.type = toolsFilesystem::getFileType(path);
	file.perm = toolsFilesystem::getPermissions(path);
	file.accTime = toolsFilesystem::getAccTime(path);
	file.modTime = toolsFilesystem::getModTime(path);
	file.size = toolsFilesystem::getSize(path);
	file.uid = toolsFilesystem::getUserOwner(path);
	file.gid = toolsFilesystem::getGroupOwner(path);

	return file;
}

//-------------------------------------------------------------------

dodoArray<__fileInfo>
toolsFilesystem::getDirInfo(const dodoString &path)
{
	dodoArray<__fileInfo> dir;
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETDIRINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!S_ISDIR(st.st_mode))
		return dir;


	DIR *directory = opendir(path.c_str());

	if (directory == NULL)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETDIRINFO, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	dirent *dd;
	dodoString attached;

	while ((dd = readdir(directory)) != NULL)
	{
		if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
			continue;
		attached.assign(path + FILE_DELIM + dd->d_name);
		dir.push_back(toolsFilesystem::getFileInfo(attached));
	}

	return dir;
}

//-------------------------------------------------------------------

dodoString
toolsFilesystem::followSymlink(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_FOLLOWSYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	char buffer[MAXPATHLEN];

	if (!S_ISLNK(st.st_mode))
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_SYMLINK, ERR_LIBDODO, TOOLSFILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

	int count = 0;

	if ((count = ::readlink(path.c_str(), buffer, MAXPATHLEN)) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_FOLLOWSYMLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return buffer;
}

//-------------------------------------------------------------------

dodoString
toolsFilesystem::getFileContents(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!S_ISREG(st.st_mode))
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_LIBDODO, TOOLSFILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

	FILE *file = fopen(path.c_str(), "r");
	if (file == NULL)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	char buffer[IO_INSIZE];

	long iter = st.st_size / IO_INSIZE, rest = st.st_size % IO_INSIZE;
	dodoString retS = "";

	int i(0);
	for (; i < iter; ++i)
	{
		if (fseek(file, i * IO_INSIZE, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		if (fread(buffer, IO_INSIZE, 1, file) == 0)
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

		retS.append(buffer, IO_INSIZE);
	}
	if (rest > 0)
	{
		if (fseek(file, i * IO_INSIZE, SEEK_SET) == -1)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

		if (fread(buffer, rest, 1, file) == 0)
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

		retS.append(buffer, rest);
	}

	if (fclose(file) != 0)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTS, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return retS;
}

//-------------------------------------------------------------------

dodoStringArray
toolsFilesystem::getFileContentsArr(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTSARR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	if (!S_ISREG(st.st_mode))
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTSARR, ERR_LIBDODO, TOOLSFILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

	FILE *file = fopen(path.c_str(), "r");
	if (file == NULL)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTSARR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	char buffer[PATH_MAXLINELEN];
	dodoStringArray arr;

	while (fgets(buffer, PATH_MAXLINELEN, file) != NULL)
		arr.push_back(buffer);

	if (fclose(file) != 0)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_GETFILECONTENTSARR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

	return arr;

}

//-------------------------------------------------------------------

dodoString
toolsFilesystem::basename(const dodoString &path)
{
	if (path.size() >= MAXPATHLEN)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_LASTNAME, ERR_LIBDODO, TOOLSFILESYSTEMEX_TOOLONGPATH, TOOLSFILESYSTEMEX_TOOLONGPATH_STR, __LINE__, __FILE__, path);

	char temp[MAXPATHLEN];

	strcpy(temp, path.c_str());

	::basename(temp);

	return temp;
}

//-------------------------------------------------------------------

dodoString
toolsFilesystem::dirname(const dodoString &path)
{
	if (path.size() >= MAXPATHLEN)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_DIRNAME, ERR_LIBDODO, TOOLSFILESYSTEMEX_TOOLONGPATH, TOOLSFILESYSTEMEX_TOOLONGPATH_STR, __LINE__, __FILE__, path);

	char temp[MAXPATHLEN];

	strcpy(temp, path.c_str());

	::dirname(temp);

	return temp;
}

//-------------------------------------------------------------------

void
toolsFilesystem::copy(const dodoString &from,
				  const dodoString &a_to,
				  bool force)
{
	dodoString to = a_to;

	{
		char temp[MAXPATHLEN];

		strcpy(temp, to.c_str());
		::basename(temp);
		if (strcmp(temp, "..") == 0  || strcmp(temp, ".") == 0 || a_to[a_to.size() - 1] == FILE_DELIM)
			to = temp + dodoString(1, FILE_DELIM) + ::basename((char *)from.c_str());
	}

	struct stat stFrom, stTo;

	if (::lstat(from.c_str(), &stFrom) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

	if (::lstat(to.c_str(), &stTo) == -1)
	{
		if (errno != ENOENT)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
	}
	else
	{
		if (force)
		{
			if (!S_ISDIR(stTo.st_mode))
			{
				if (::unlink(to.c_str()) == -1)
					throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
			}
			else
			if (::rmdir(to.c_str()) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_UNLINK, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

		}
		else
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_LIBDODO, TOOLSFILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, to);
	}

	if (!S_ISREG(stFrom.st_mode))
	{
		if (S_ISDIR(stFrom.st_mode))
		{
			if (::mkdir(to.c_str(), stFrom.st_mode) == 1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
		}
		else
		if (S_ISLNK(stFrom.st_mode))
		{
			char buffer[MAXPATHLEN];
			int count = 0;

			if ((count = ::readlink(from.c_str(), buffer, MAXPATHLEN)) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

			buffer[count] = '\0';

			if (::symlink(buffer, to.c_str()) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, dodoString(buffer) + "->" + to);
		}
		else
		if (::mknod(to.c_str(), stFrom.st_mode, 0) == 1)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
	}
	else
	{
		long iter = stFrom.st_size / IO_INSIZE, rest = stFrom.st_size % IO_INSIZE;

		FILE *fromFile = fopen(from.c_str(), "r");
		if (fromFile == NULL)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

		FILE *toFile = fopen(to.c_str(), "w+");
		if (toFile == NULL)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

		char buffer[IO_INSIZE];

		int i(0), j;
		for (; i < iter; ++i)
		{
			j = i * IO_INSIZE;

			if (fseek(fromFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fseek(toFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fread(buffer, IO_INSIZE, 1, fromFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}

			if (fwrite(buffer, IO_INSIZE, 1, toFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
		}
		if (rest > 0)
		{
			j = i * IO_INSIZE;
			if (fseek(fromFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fseek(toFile, j, SEEK_SET) == -1)
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

			if (fread(buffer, rest, 1, fromFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}

			if (fwrite(buffer, rest, 1, toFile) == 0)
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
		}

		if (fclose(fromFile) != 0)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

		if (fclose(toFile) != 0)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPY, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
	}
}

//-------------------------------------------------------------------

void
toolsFilesystem::copyDir(const dodoString &from,
					 const dodoString &a_to,
					 bool force)
{
	dodoString to = a_to;

	{
		char temp[MAXPATHLEN];

		strcpy(temp, to.c_str());
		::basename(temp);
		if (strcmp(temp, "..") == 0 || strcmp(temp, ".") == 0 || a_to[a_to.size() - 1] == FILE_DELIM)
			to = temp + dodoString(1, FILE_DELIM) + ::basename((char *)from.c_str());
	}

	struct stat stFrom, stTo;

	if (::lstat(from.c_str(), &stFrom) == -1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

	if (::lstat(to.c_str(), &stTo) == -1)
	{
		if (errno != ENOENT)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
	}
	else
	if (force)
		toolsFilesystem::rm(to, force);
	else
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPYDIR, ERR_LIBDODO, TOOLSFILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, to);

	if (!S_ISDIR(stFrom.st_mode))
		toolsFilesystem::copy(from, to, force);
	else
	{
		if (::mkdir(to.c_str(), stFrom.st_mode) == -1)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

		dodoString attachedFrom, attachedTo;

		DIR *directory = opendir(from.c_str());
		if (directory == NULL)
		{
			if (errno == ENOENT)
				return ;
			else
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
		}

		dirent *dd;

		while ((dd = readdir(directory)) != NULL)
		{
			if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
				continue;

			attachedTo.assign(to + FILE_DELIM + dd->d_name);
			attachedFrom.assign(from + FILE_DELIM + dd->d_name);

			toolsFilesystem::copyDir(attachedFrom, attachedTo, force);
		}

		if (closedir(directory) == -1)
			throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX_COPYDIR, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
	}
}

//-------------------------------------------------------------------

bool
toolsFilesystem::exists(const dodoString &path)
{
	struct stat st;

	if (::lstat(path.c_str(), &st) == 0)
		return true;

	return false;
}

//-------------------------------------------------------------------

void
toolsFilesystem::appendToFile(const dodoString &path,
						const dodoString &content)
{
	_writeToFile(path, content, "a+");
}

//-------------------------------------------------------------------

void
toolsFilesystem::appendToFile(const dodoString &path,
						const dodoStringArray &content)
{
	_writeToFile(path, content, "a+");
}

//-------------------------------------------------------------------

void
toolsFilesystem::writeToFile(const dodoString &path,
						const dodoString &content)
{
	_writeToFile(path, content, "w+");
}

//-------------------------------------------------------------------

void
toolsFilesystem::writeToFile(const dodoString &path,
						const dodoStringArray &content)
{
	_writeToFile(path, content, "w+");
}

//-------------------------------------------------------------------

void
toolsFilesystem::_writeToFile(const dodoString &path,
                                        const dodoString &content,
                                        const char *mode)
{
	FILE *file = fopen(path.c_str(), mode);
	if (file == NULL)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX__WRITETOFILE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	
	unsigned long size = content.size();
	if (fwrite(content.c_str(), size, 1, file) != 1)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX__WRITETOFILE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
toolsFilesystem::_writeToFile(const dodoString &path,
                                        const dodoStringArray &content,
                                        const char *mode)
{
	FILE *file = fopen(path.c_str(), mode);
	if (file == NULL)
		throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX__WRITETOFILE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	
	dodoStringArray::const_iterator i=content.begin(), j=content.end();
	for (;i!=j;++i)
	{
	if (fputs(i->c_str(), file) < 0)
		switch (errno)
		{   
			case EFBIG:
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOSPC:
			case EPIPE:
			case ENOMEM:
			case ENXIO:
			
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX__WRITETOFILE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}   
	
	if (fputc('\n', file) < 0)
		switch (errno)
		{   
			case EFBIG:
			case EIO:
			case EINTR:
			case EBADF:
			case EOVERFLOW:
			case ENOSPC:
			case EPIPE:
			case ENOMEM:
			case ENXIO:
		
				throw baseEx(ERRMODULE_TOOLSFILESYSTEM, TOOLSFILESYSTEMEX__WRITETOFILE, ERR_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
	}
}

//-------------------------------------------------------------------
