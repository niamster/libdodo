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

/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */

#include <libdodo/directives.h>

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <utime.h>
#include <time.h>
#include <libgen.h>
#include <unistd.h>
#include <errno.h>

#include <libdodo/toolsFilesystem.h>
#include <libdodo/toolsFilesystemEx.h>
#include <libdodo/types.h>

using namespace dodo::tools;

void
filesystem::unlink(const dodoString &path,
				   bool             force)
{
	int status(0);
	struct stat st;

	if (::lstat(path.c_str(), &st) == -1)
	{
		if (errno != ENOENT || !force)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
	}

	if (S_ISDIR(st.st_mode))
	{
		status = ::rmdir(path.c_str());
	}
	else
	{
		status = ::unlink(path.c_str());
	}

	if (status == -1)
	{
		if (errno != ENOENT || !force)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
	}
}

//-------------------------------------------------------------------

void
filesystem::rename(const dodoString &oldPath,
				   const dodoString &newPath)
{
	if (::rename(oldPath.c_str(), newPath.c_str()) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RENAME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
	}
}

//-------------------------------------------------------------------

void
filesystem::symlink(const dodoString &oldPath,
					const dodoString &newPath,
					bool             force)
{
	if (force)
	{
		struct stat st;
		if (::lstat(newPath.c_str(), &st) != -1)
		{
			if (!S_ISLNK(st.st_mode))
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, newPath);
			}
			else if (::unlink(newPath.c_str()) == -1)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, newPath);
			}
		}
	}

	if (::symlink(oldPath.c_str(), newPath.c_str()) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
	}
}

//-------------------------------------------------------------------

void
filesystem::link(const dodoString &oldPath,
				 const dodoString &newPath)
{
	if  (::link(oldPath.c_str(), newPath.c_str()) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_LINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
	}
}

//-------------------------------------------------------------------

void
filesystem::chown(const dodoString &path,
				  int              uid)
{
	if (::chown(path.c_str(), uid, (unsigned int)-1) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_CHOWN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

void
filesystem::chgrp(const dodoString &path,
				  int              gid)
{
	if (::chown(path.c_str(), (unsigned int)-1, gid) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_CHGRP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

int
filesystem::getUserOwner(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETUSEROWNER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return st.st_uid;
}

//-------------------------------------------------------------------

int
filesystem::getGroupOwner(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETGROUPOWNER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return st.st_gid;
}

//-------------------------------------------------------------------

void
filesystem::touch(const dodoString &path,
				  int              a_time)
{
	if (a_time == 1)
	{
		a_time = time(NULL);
	}

	utimbuf temp =
	{
		a_time, a_time
	};

	if (::utime(path.c_str(), &temp) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_TOUCH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

void
filesystem::mkfifo(const dodoString &path,
				   int              permissions)
{
	if (::mkfifo(path.c_str(), toRealPermission(permissions)) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKFIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

void
filesystem::mkdir(const dodoString &path,
				  int              permissions)
{
	if (::mkdir(path.c_str(), toRealPermission(permissions)) == -1)
	{
		if (errno == EEXIST)
		{
			struct stat st;
			if (::lstat(path.c_str(), &st) == -1)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

			if (!S_ISDIR(st.st_mode))
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_LIBDODO, FILESYSTEMEX_NOTADIR, TOOLSFILESYSTEMEX_NOTADIR_STR, __LINE__, __FILE__, path);
			}
		}
		else
		{
			mkdir(dirname(path), permissions);

			if (::mkdir(path.c_str(), toRealPermission(permissions)) == -1)
			{
				if (errno == EEXIST)
				{
					struct stat st;
					if (::lstat(path.c_str(), &st) == -1)
					{
						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
					}

					if (!S_ISDIR(st.st_mode))
					{
						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_LIBDODO, FILESYSTEMEX_NOTADIR, TOOLSFILESYSTEMEX_NOTADIR_STR, __LINE__, __FILE__, path);
					}
				}
			}
		}
	}
}

//-------------------------------------------------------------------

void
filesystem::chmod(const dodoString &path,
				  int              permissions)
{
	if (::chmod(path.c_str(), toRealPermission(permissions)) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_CHMOD, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

int
filesystem::toRealPermission(int permission)
{
	int mode(0);

	if (isSetFlag(permission, FILESYSTEM_PERMISSION_OWNER_READ_ACCESS))
	{
		mode |= S_IRUSR;
	}
	if (isSetFlag(permission, FILESYSTEM_PERMISSION_GROUP_READ_ACCESS))
	{
		mode |= S_IRGRP;
	}
	if (isSetFlag(permission, FILESYSTEM_PERMISSION_OTHER_READ_ACCESS))
	{
		mode |= S_IROTH;
	}

	if (isSetFlag(permission, FILESYSTEM_PERMISSION_OWNER_WRITE_ACCESS))
	{
		mode |= S_IWUSR;
	}
	if (isSetFlag(permission, FILESYSTEM_PERMISSION_GROUP_WRITE_ACCESS))
	{
		mode |= S_IWGRP;
	}
	if (isSetFlag(permission, FILESYSTEM_PERMISSION_OTHER_WRITE_ACCESS))
	{
		mode |= S_IWOTH;
	}

	if (isSetFlag(permission, FILESYSTEM_PERMISSION_OWNER_EXECUTE_ACCESS))
	{
		mode |= S_IXUSR;
	}
	if (isSetFlag(permission, FILESYSTEM_PERMISSION_GROUP_EXECUTE_ACCESS))
	{
		mode |= S_IXGRP;
	}
	if (isSetFlag(permission, FILESYSTEM_PERMISSION_OTHER_EXECUTE_ACCESS))
	{
		mode |= S_IXOTH;
	}

	if (isSetFlag(permission, FILESYSTEM_PERMISSION_STICKY_ACCESS))
	{
		mode |= S_ISVTX;
	}

	if (isSetFlag(permission, FILESYSTEM_PERMISSION_SUID_ACCESS))
	{
		mode |= S_ISUID;
	}
	if (isSetFlag(permission, FILESYSTEM_PERMISSION_SGID_ACCESS))
	{
		mode |= S_ISGID;
	}

	return mode;
}

//-------------------------------------------------------------------

void
filesystem::rm(const dodoString &path,
			   bool             force)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		if (errno != ENOENT || !force)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
		}
		else
		{
			return ;
		}
	}

	if (!S_ISDIR(st.st_mode))
	{
		if (::unlink(path.c_str()) == -1)
		{
			if (errno != ENOENT || !force)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}
	}
	else
	{
		dodoString attached;

		DIR *directory = opendir(path.c_str());

		if (directory == NULL)
		{
			if (errno != ENOENT || !force)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}
		else
		{
			dirent *dd;

			while ((dd = readdir(directory)) != NULL)
			{
				if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
				{
					continue;
				}

				attached.assign(path + FILE_DELIM + dd->d_name);

				if (::lstat(attached.c_str(), &st) == -1)
				{
					if (errno != ENOENT || !force)
					{
						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
					}
				}

				if (S_ISDIR(st.st_mode))
				{
					filesystem::rm(attached.c_str());
				}
				else if (::unlink(attached.c_str()) == -1)
				{
					if (errno != ENOENT || !force)
					{
						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
					}
				}
			}

			if (closedir(directory) == -1)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}

			if (::rmdir(path.c_str()) == -1)
			{
				if (errno != ENOENT || !force)
				{
					throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
				}
			}
		}
	}
}

//-------------------------------------------------------------------

int
filesystem::getPermissions(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETPERMISSIONS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	int mode(FILESYSTEM_PERMISSION_NONE);

	if (isSetFlag(st.st_mode, S_IRUSR))
	{
		mode |= FILESYSTEM_PERMISSION_OWNER_READ_ACCESS;
	}
	if (isSetFlag(st.st_mode, S_IRGRP))
	{
		mode |= FILESYSTEM_PERMISSION_GROUP_READ_ACCESS;
	}
	if (isSetFlag(st.st_mode, S_IROTH))
	{
		mode |= FILESYSTEM_PERMISSION_OTHER_READ_ACCESS;
	}

	if (isSetFlag(st.st_mode, S_IWUSR))
	{
		mode |= FILESYSTEM_PERMISSION_OWNER_WRITE_ACCESS;
	}
	if (isSetFlag(st.st_mode, S_IWGRP))
	{
		mode |= FILESYSTEM_PERMISSION_GROUP_WRITE_ACCESS;
	}
	if (isSetFlag(st.st_mode, S_IWOTH))
	{
		mode |= FILESYSTEM_PERMISSION_OTHER_WRITE_ACCESS;
	}

	if (isSetFlag(st.st_mode, S_ISVTX))
	{
		mode |= FILESYSTEM_PERMISSION_STICKY_ACCESS;
	}

	if (isSetFlag(st.st_mode, S_IXUSR))
	{
		mode |= FILESYSTEM_PERMISSION_OWNER_EXECUTE_ACCESS;
	}
	if (isSetFlag(st.st_mode, S_IXGRP))
	{
		mode |= FILESYSTEM_PERMISSION_GROUP_EXECUTE_ACCESS;
	}
	if (isSetFlag(st.st_mode, S_IXOTH))
	{
		mode |= FILESYSTEM_PERMISSION_OTHER_EXECUTE_ACCESS;
	}

	if (isSetFlag(st.st_mode, S_ISUID))
	{
		mode |= FILESYSTEM_PERMISSION_SUID_ACCESS;
	}
	if (isSetFlag(st.st_mode, S_ISGID))
	{
		mode |= FILESYSTEM_PERMISSION_SGID_ACCESS;
	}

	return mode;
}

//-------------------------------------------------------------------

int
filesystem::getFileType(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILETYPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	st.st_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);

	switch (st.st_mode)
	{
		case S_IFREG:

			return FILESYSTEM_FILETYPE_REGULAR_FILE;

		case S_IFDIR:

			return FILESYSTEM_FILETYPE_DIRECTORY;

		case S_IFLNK:

			return FILESYSTEM_FILETYPE_SYMBOLIC_LINK;

		case S_IFSOCK:

			return FILESYSTEM_FILETYPE_LOCAL_SOCKET;

		case S_IFBLK:

			return FILESYSTEM_FILETYPE_BLOCK_DEVICE;

		case S_IFCHR:

			return FILESYSTEM_FILETYPE_CHARACTER_DEVICE;

		case S_IFIFO:

			return FILESYSTEM_FILETYPE_FIFO;

		default:

			return -1;
	}
}

//-------------------------------------------------------------------

long
filesystem::getSize(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETSIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return st.st_size;
}

//-------------------------------------------------------------------

long
filesystem::getAccTime(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETACCTIME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return st.st_atime;
}

//-------------------------------------------------------------------

long
filesystem::getModTime(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETMODTIME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return st.st_mtime;
}

//-------------------------------------------------------------------

__fileInfo__
filesystem::getFileInfo(const dodoString &path)
{
	__fileInfo__ file;

	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	file.name.assign(::basename((char *)path.c_str()));
	file.type = filesystem::getFileType(path);
	file.perm = filesystem::getPermissions(path);
	file.accTime = filesystem::getAccTime(path);
	file.modTime = filesystem::getModTime(path);
	file.size = filesystem::getSize(path);
	file.uid = filesystem::getUserOwner(path);
	file.gid = filesystem::getGroupOwner(path);

	return file;
}

//-------------------------------------------------------------------

dodoArray<__fileInfo__>
filesystem::getDirInfo(const dodoString &path)
{
	dodoArray<__fileInfo__> dir;
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETDIRINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	if (!S_ISDIR(st.st_mode))
	{
		return dir;
	}


	DIR *directory = opendir(path.c_str());

	if (directory == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETDIRINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	dirent *dd;
	dodoString attached;

	while ((dd = readdir(directory)) != NULL)
	{
		if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
		{
			continue;
		}
		attached.assign(path + FILE_DELIM + dd->d_name);
		dir.push_back(filesystem::getFileInfo(attached));
	}

	return dir;
}

//-------------------------------------------------------------------

dodoString
filesystem::followSymlink(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FOLLOWSYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	char buffer[MAXPATHLEN + 1];

	if (!S_ISLNK(st.st_mode))
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	}

	int count = 0;

	if ((count = ::readlink(path.c_str(), buffer, MAXPATHLEN)) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FOLLOWSYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return buffer;
}

//-------------------------------------------------------------------

dodoString
filesystem::getFileContents(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	if (!S_ISREG(st.st_mode))
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTS, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	}

	FILE *file = fopen(path.c_str(), "r");
	if (file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	char buffer[IO_INSIZE];

	long iter = st.st_size / IO_INSIZE, rest = st.st_size % IO_INSIZE;
	dodoString retS = "";

	int i(0);
	for (; i < iter; ++i)
	{
		if (fread(buffer, IO_INSIZE, 1, file) == 0)
		{
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}

		retS.append(buffer, IO_INSIZE);
	}
	if (rest > 0)
	{
		if (fread(buffer, rest, 1, file) == 0)
		{
			switch (errno)
			{
				case EIO:
				case EINTR:
				case ENOMEM:
				case EOVERFLOW:
				case EROFS:

					throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}

		retS.append(buffer, rest);
	}

	if (fclose(file) != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return retS;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
filesystem::getFileContentsArr(const dodoString &path)
{
	struct stat st;
	if (::lstat(path.c_str(), &st) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTSARR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	if (!S_ISREG(st.st_mode))
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTSARR, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);
	}

	FILE *file = fopen(path.c_str(), "r");
	if (file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTSARR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	char buffer[PATH_MAXLINELEN];
	dodoStringArray arr;

	while (fgets(buffer, PATH_MAXLINELEN, file) != NULL)
	{
		arr.push_back(buffer);
	}

	if (fclose(file) != 0)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GETFILECONTENTSARR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	return arr;
}

//-------------------------------------------------------------------

dodoString
filesystem::basename(const dodoString &path)
{
	if (path.size() >= MAXPATHLEN)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_LASTNAME, exception::ERRNO_LIBDODO, FILESYSTEMEX_TOOLONGPATH, TOOLSFILESYSTEMEX_TOOLONGPATH_STR, __LINE__, __FILE__, path);
	}

	char temp[MAXPATHLEN];

	strcpy(temp, path.c_str());

	return ::basename(temp);
}

//-------------------------------------------------------------------

dodoString
filesystem::dirname(const dodoString &path)
{
	if (path.size() >= MAXPATHLEN)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_DIRNAME, exception::ERRNO_LIBDODO, FILESYSTEMEX_TOOLONGPATH, TOOLSFILESYSTEMEX_TOOLONGPATH_STR, __LINE__, __FILE__, path);
	}

	char temp[MAXPATHLEN];

	strcpy(temp, path.c_str());

	return ::dirname(temp);
}

//-------------------------------------------------------------------

void
filesystem::copy(const dodoString &from,
				 const dodoString &a_to,
				 bool             force)
{
	dodoString to = a_to;

	{
		char temp[MAXPATHLEN];

		strcpy(temp, to.c_str());
		char *bname = ::basename(temp);
		if (strcmp(bname, "..") == 0  || strcmp(bname, ".") == 0 || a_to[a_to.size() - 1] == FILE_DELIM)
		{
			to = bname + dodoString(1, FILE_DELIM) + ::basename((char *)from.c_str());
		}
	}

	struct stat stFrom, stTo;

	if (::lstat(from.c_str(), &stFrom) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
	}

	if (::lstat(to.c_str(), &stTo) == -1)
	{
		if (errno != ENOENT)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
		}
	}
	else
	{
		if (force)
		{
			if (!S_ISDIR(stTo.st_mode))
			{
				if (::unlink(to.c_str()) == -1)
				{
					throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
				}
			}
			else if (::rmdir(to.c_str()) == -1)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
			}
		}
		else
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, to);
		}
	}

	if (!S_ISREG(stFrom.st_mode))
	{
		if (S_ISDIR(stFrom.st_mode))
		{
			if (::mkdir(to.c_str(), stFrom.st_mode) == 1)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
			}
		}
		else if (S_ISLNK(stFrom.st_mode))
		{
			char buffer[MAXPATHLEN];
			int count = 0;

			if ((count = ::readlink(from.c_str(), buffer, MAXPATHLEN)) == -1)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
			}

			buffer[count] = '\0';

			if (::symlink(buffer, to.c_str()) == -1)
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, dodoString(buffer) + "->" + to);
			}
		}
		else if (::mknod(to.c_str(), stFrom.st_mode, 0) == 1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
		}
	}
	else
	{
		long iter = stFrom.st_size / IO_INSIZE, rest = stFrom.st_size % IO_INSIZE;

		FILE *fromFile = fopen(from.c_str(), "r");
		if (fromFile == NULL)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
		}

		FILE *toFile = fopen(to.c_str(), "w+");
		if (toFile == NULL)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
		}

		char buffer[IO_INSIZE];

		int i(0);
		for (; i < iter; ++i)
		{
			if (fread(buffer, IO_INSIZE, 1, fromFile) == 0)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
			}

			if (fwrite(buffer, IO_INSIZE, 1, toFile) == 0)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
			}
		}
		if (rest > 0)
		{
			if (fread(buffer, rest, 1, fromFile) == 0)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
			}

			if (fwrite(buffer, rest, 1, toFile) == 0)
			{
				switch (errno)
				{
					case EIO:
					case EINTR:
					case ENOMEM:
					case EOVERFLOW:
					case EROFS:

						throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
				}
			}
		}

		if (fclose(fromFile) != 0)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
		}

		if (fclose(toFile) != 0)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
		}
	}
}

//-------------------------------------------------------------------

void
filesystem::copyDir(const dodoString &from,
					const dodoString &a_to,
					bool             force)
{
	dodoString to = a_to;

	{
		char temp[MAXPATHLEN];

		strcpy(temp, to.c_str());
		char *bname = ::basename(temp);
		if (strcmp(bname, "..") == 0 || strcmp(bname, ".") == 0 || a_to[a_to.size() - 1] == FILE_DELIM)
		{
			to = bname + dodoString(1, FILE_DELIM) + ::basename((char *)from.c_str());
		}
	}

	struct stat stFrom, stTo;

	if (::lstat(from.c_str(), &stFrom) == -1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
	}

	if (::lstat(to.c_str(), &stTo) == -1)
	{
		if (errno != ENOENT)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
		}
	}
	else if (force)
	{
		filesystem::rm(to, force);
	}
	else
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, to);
	}

	if (!S_ISDIR(stFrom.st_mode))
	{
		filesystem::copy(from, to, force);
	}
	else
	{
		if (::mkdir(to.c_str(), stFrom.st_mode) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
		}

		dodoString attachedFrom, attachedTo;

		DIR *directory = opendir(from.c_str());
		if (directory == NULL)
		{
			if (errno == ENOENT)
			{
				return ;
			}
			else
			{
				throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
			}
		}

		dirent *dd;

		while ((dd = readdir(directory)) != NULL)
		{
			if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
			{
				continue;
			}

			attachedTo.assign(to + FILE_DELIM + dd->d_name);
			attachedFrom.assign(from + FILE_DELIM + dd->d_name);

			filesystem::copyDir(attachedFrom, attachedTo, force);
		}

		if (closedir(directory) == -1)
		{
			throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
		}
	}
}

//-------------------------------------------------------------------

bool
filesystem::exists(const dodoString &path)
{
	struct stat st;

	if (::lstat(path.c_str(), &st) == 0)
	{
		return true;
	}

	return false;
}

//-------------------------------------------------------------------

void
filesystem::appendToFile(const dodoString &path,
						 const dodoString &content)
{
	_writeToFile(path, content, "a+");
}

//-------------------------------------------------------------------

void
filesystem::appendToFile(const dodoString      &path,
						 const dodoStringArray &content)
{
	_writeToFile(path, content, "a+");
}

//-------------------------------------------------------------------

void
filesystem::writeToFile(const dodoString &path,
						const dodoString &content)
{
	_writeToFile(path, content, "w+");
}

//-------------------------------------------------------------------

void
filesystem::writeToFile(const dodoString      &path,
						const dodoStringArray &content)
{
	_writeToFile(path, content, "w+");
}

//-------------------------------------------------------------------

void
filesystem::_writeToFile(const dodoString &path,
						 const dodoString &content,
						 const char       *mode)
{
	FILE *file = fopen(path.c_str(), mode);
	if (file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	unsigned long size = content.size();
	if (size > 0 && fwrite(content.c_str(), size, 1, file) != 1)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}
}

//-------------------------------------------------------------------

void
filesystem::_writeToFile(const dodoString      &path,
						 const dodoStringArray &content,
						 const char            *mode)
{
	FILE *file = fopen(path.c_str(), mode);
	if (file == NULL)
	{
		throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
	}

	dodoStringArray::const_iterator i = content.begin(), j = content.end();
	for (; i != j; ++i)
	{
		if (i->size() > 0 && fputs(i->c_str(), file) < 0)
		{
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

					throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}

		if (fputc('\n', file) < 0)
		{
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

					throw exception::basic(exception::ERRMODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE__, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
			}
		}
	}
}

//-------------------------------------------------------------------
