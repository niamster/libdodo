/***************************************************************************
 *            toolsFilesystem.cc
 *
 *  Wed Oct 8 2005
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
#include <stdio.h>

#include <libdodo/toolsFilesystem.h>
#include <libdodo/toolsFilesystemEx.h>
#include <libdodo/types.h>

using namespace dodo::tools;

void
filesystem::unlink(const dodo::string &path,
                   bool             force)
{
    int status(0);
    struct stat st;

    if (::lstat(path.data(), &st) == -1)
        if (errno != ENOENT || !force)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    if (S_ISDIR(st.st_mode))
        status = ::rmdir(path.data());
    else
        status = ::unlink(path.data());

    if (status == -1)
        if (errno != ENOENT || !force)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
filesystem::rename(const dodo::string &oldPath,
                   const dodo::string &newPath)
{
    if (::rename(oldPath.data(), newPath.data()) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RENAME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
filesystem::symlink(const dodo::string &oldPath,
                    const dodo::string &newPath,
                    bool             force)
{
    if (force) {
        struct stat st;
        if (::lstat(newPath.data(), &st) != -1) {
            if (!S_ISLNK(st.st_mode))
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, newPath);
            else if (::unlink(newPath.data()) == -1)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, newPath);
        }
    }

    if (::symlink(oldPath.data(), newPath.data()) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
filesystem::link(const dodo::string &oldPath,
                 const dodo::string &newPath)
{
    if  (::link(oldPath.data(), newPath.data()) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_LINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, oldPath + "->" + newPath);
}

//-------------------------------------------------------------------

void
filesystem::chown(const dodo::string &path,
                  int              uid)
{
    if (::chown(path.data(), uid, (unsigned int)-1) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_CHOWN, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
filesystem::chgrp(const dodo::string &path,
                  int              gid)
{
    if (::chown(path.data(), (unsigned int)-1, gid) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_CHGRP, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

int
filesystem::userOwner(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_USEROWNER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return st.st_uid;
}

//-------------------------------------------------------------------

int
filesystem::groupOwner(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_GROUPOWNER, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return st.st_gid;
}

//-------------------------------------------------------------------

void
filesystem::touch(const dodo::string &path,
                  int              a_time)
{
    if (a_time == 1)
        a_time = time(NULL);

    utimbuf temp = {
        a_time, a_time
    };

    if (::utime(path.data(), &temp) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_TOUCH, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
filesystem::mkfifo(const dodo::string &path,
                   int              permissions)
{
    if (::mkfifo(path.data(), toRealPermission(permissions)) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKFIFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
filesystem::mkdir(const dodo::string &path,
                  int              permissions)
{
    if (::mkdir(path.data(), toRealPermission(permissions)) == -1) {
        if (errno == EEXIST) {
            struct stat st;
            if (::lstat(path.data(), &st) == -1)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

            if (!S_ISDIR(st.st_mode))
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_LIBDODO, FILESYSTEMEX_NOTADIR, TOOLSFILESYSTEMEX_NOTADIR_STR, __LINE__, __FILE__, path);
        } else {
            mkdir(dirname(path), permissions);

            if (::mkdir(path.data(), toRealPermission(permissions)) == -1) {
                if (errno == EEXIST) {
                    struct stat st;
                    if (::lstat(path.data(), &st) == -1)
                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

                    if (!S_ISDIR(st.st_mode))
                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MKDIR, exception::ERRNO_LIBDODO, FILESYSTEMEX_NOTADIR, TOOLSFILESYSTEMEX_NOTADIR_STR, __LINE__, __FILE__, path);
                }
            }
        }
    }
}

//-------------------------------------------------------------------

void
filesystem::chmod(const dodo::string &path,
                  int              permissions)
{
    if (::chmod(path.data(), toRealPermission(permissions)) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_CHMOD, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

int
filesystem::toRealPermission(int permission)
{
    int mode(0);

    if (isSetFlag(permission, PERMISSION_OWNER_READ_ACCESS))
        mode |= S_IRUSR;
    if (isSetFlag(permission, PERMISSION_GROUP_READ_ACCESS))
        mode |= S_IRGRP;
    if (isSetFlag(permission, PERMISSION_OTHER_READ_ACCESS))
        mode |= S_IROTH;

    if (isSetFlag(permission, PERMISSION_OWNER_WRITE_ACCESS))
        mode |= S_IWUSR;
    if (isSetFlag(permission, PERMISSION_GROUP_WRITE_ACCESS))
        mode |= S_IWGRP;
    if (isSetFlag(permission, PERMISSION_OTHER_WRITE_ACCESS))
        mode |= S_IWOTH;

    if (isSetFlag(permission, PERMISSION_OWNER_EXECUTE_ACCESS))
        mode |= S_IXUSR;
    if (isSetFlag(permission, PERMISSION_GROUP_EXECUTE_ACCESS))
        mode |= S_IXGRP;
    if (isSetFlag(permission, PERMISSION_OTHER_EXECUTE_ACCESS))
        mode |= S_IXOTH;

    if (isSetFlag(permission, PERMISSION_STICKY_ACCESS))
        mode |= S_ISVTX;

    if (isSetFlag(permission, PERMISSION_SUID_ACCESS))
        mode |= S_ISUID;
    if (isSetFlag(permission, PERMISSION_SGID_ACCESS))
        mode |= S_ISGID;

    return mode;
}

//-------------------------------------------------------------------

void
filesystem::rm(const dodo::string &path,
               bool             force)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1) {
        if (errno != ENOENT || !force)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
        else
            return ;
    }

    if (!S_ISDIR(st.st_mode)) {
        if (::unlink(path.data()) == -1)
            if (errno != ENOENT || !force)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
    } else {
        dodo::string attached;

        DIR *directory = opendir(path.data());

        if (directory == NULL) {
            if (errno != ENOENT || !force)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
        } else {
            dirent *dd;

            while ((dd = readdir(directory)) != NULL) {
                if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
                    continue;

                attached = dodo::string(path + "/" + dd->d_name);

                if (::lstat(attached.data(), &st) == -1)
                    if (errno != ENOENT || !force)
                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

                if (S_ISDIR(st.st_mode))
                    filesystem::rm(attached.data());
                else if (::unlink(attached.data()) == -1)
                    if (errno != ENOENT || !force)
                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
            }

            if (closedir(directory) == -1)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

            if (::rmdir(path.data()) == -1)
                if (errno != ENOENT || !force)
                    dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_RM, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
        }
    }
}

//-------------------------------------------------------------------

int
filesystem::permissions(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_PERMISSIONS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    int mode(PERMISSION_NONE);

    if (isSetFlag(st.st_mode, S_IRUSR))
        mode |= PERMISSION_OWNER_READ_ACCESS;
    if (isSetFlag(st.st_mode, S_IRGRP))
        mode |= PERMISSION_GROUP_READ_ACCESS;
    if (isSetFlag(st.st_mode, S_IROTH))
        mode |= PERMISSION_OTHER_READ_ACCESS;

    if (isSetFlag(st.st_mode, S_IWUSR))
        mode |= PERMISSION_OWNER_WRITE_ACCESS;
    if (isSetFlag(st.st_mode, S_IWGRP))
        mode |= PERMISSION_GROUP_WRITE_ACCESS;
    if (isSetFlag(st.st_mode, S_IWOTH))
        mode |= PERMISSION_OTHER_WRITE_ACCESS;

    if (isSetFlag(st.st_mode, S_ISVTX))
        mode |= PERMISSION_STICKY_ACCESS;

    if (isSetFlag(st.st_mode, S_IXUSR))
        mode |= PERMISSION_OWNER_EXECUTE_ACCESS;
    if (isSetFlag(st.st_mode, S_IXGRP))
        mode |= PERMISSION_GROUP_EXECUTE_ACCESS;
    if (isSetFlag(st.st_mode, S_IXOTH))
        mode |= PERMISSION_OTHER_EXECUTE_ACCESS;

    if (isSetFlag(st.st_mode, S_ISUID))
        mode |= PERMISSION_SUID_ACCESS;
    if (isSetFlag(st.st_mode, S_ISGID))
        mode |= PERMISSION_SGID_ACCESS;

    return mode;
}

//-------------------------------------------------------------------

int
filesystem::fileType(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILETYPE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    st.st_mode &= ~(S_IRWXU | S_IRWXG | S_IRWXO | S_ISUID | S_ISGID | S_ISVTX);

    switch (st.st_mode) {
        case S_IFREG:

            return FILE_REGULAR_FILE;

        case S_IFDIR:

            return FILE_DIRECTORY;

        case S_IFLNK:

            return FILE_SYMBOLIC_LINK;

        case S_IFSOCK:

            return FILE_LOCAL_SOCKET;

        case S_IFBLK:

            return FILE_BLOCK_DEVICE;

        case S_IFCHR:

            return FILE_CHARACTER_DEVICE;

        case S_IFIFO:

            return FILE_FIFO;

        default:

            return -1;
    }
}

//-------------------------------------------------------------------

long
filesystem::size(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SIZE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return st.st_size;
}

//-------------------------------------------------------------------

long
filesystem::accessTime(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_ACCTIME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return st.st_atime;
}

//-------------------------------------------------------------------

long
filesystem::modificationTime(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_MODTIME, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return st.st_mtime;
}

//-------------------------------------------------------------------

filesystem::__file__
filesystem::file(const dodo::string &path)
{
    __file__ file;

    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILEINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    file.name = dodo::string(::basename((char *)path.data()));
    file.type = filesystem::fileType(path);
    file.perm = filesystem::permissions(path);
    file.accTime = filesystem::accessTime(path);
    file.modTime = filesystem::modificationTime(path);
    file.size = filesystem::size(path);
    file.uid = filesystem::userOwner(path);
    file.gid = filesystem::groupOwner(path);

    return file;
}

//-------------------------------------------------------------------

dodoArray<filesystem::__file__>
filesystem::dir(const dodo::string &path)
{
    dodoArray<__file__> dir;
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_DIRINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    if (!S_ISDIR(st.st_mode))
        return dir;

    DIR *directory = opendir(path.data());

    if (directory == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_DIRINFO, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    dirent *dd;
    dodo::string attached;

    while ((dd = readdir(directory)) != NULL) {
        if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
            continue;
        attached = dodo::string(path + "/" + dd->d_name);
        dir.push_back(filesystem::file(attached));
    }

    return dir;
}

//-------------------------------------------------------------------

dodo::string
filesystem::followSymlink(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FOLLOWSYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    char buffer[PATH_MAXLEN + 1];

    if (!S_ISLNK(st.st_mode))
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_SYMLINK, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

    int count = 0;

    if ((count = ::readlink(path.data(), buffer, PATH_MAXLEN)) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FOLLOWSYMLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return buffer;
}

//-------------------------------------------------------------------

dodo::string
filesystem::fileContents(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    if (!S_ISREG(st.st_mode))
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTS, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

    FILE *file = fopen(path.data(), "r");
    if (file == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    char buffer[IO_BLOCKSIZE];

    long iter = st.st_size / IO_BLOCKSIZE, rest = st.st_size % IO_BLOCKSIZE;
    dodo::string retS = "";

    int i(0);
    for (; i < iter; ++i) {
        if (fread(buffer, IO_BLOCKSIZE, 1, file) == 0) {
            switch (errno) {
                case EIO:
                case EINTR:
                case ENOMEM:
                case EOVERFLOW:
                case EROFS:

                    dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
            }
        }

        retS += dodo::string(buffer, IO_BLOCKSIZE);
    }
    if (rest > 0) {
        if (fread(buffer, rest, 1, file) == 0) {
            switch (errno) {
                case EIO:
                case EINTR:
                case ENOMEM:
                case EOVERFLOW:
                case EROFS:

                    dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
            }
        }

        retS += dodo::string(buffer, rest);
    }

    if (fclose(file) != 0)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTS, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return retS;
}

//-------------------------------------------------------------------

dodo::dodoStringArray
filesystem::fileContentsInArray(const dodo::string &path)
{
    struct stat st;
    if (::lstat(path.data(), &st) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTSARR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    if (!S_ISREG(st.st_mode))
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTSARR, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, path);

    FILE *file = fopen(path.data(), "r");
    if (file == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTSARR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    char buffer[LINE_MAXLEN];
    dodoStringArray arr;

    while (fgets(buffer, LINE_MAXLEN, file) != NULL)
        arr.push_back(buffer);

    if (fclose(file) != 0)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_FILECONTENTSARR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    return arr;
}

//-------------------------------------------------------------------

#ifdef basename
inline char *
original_basename(char *path)
{
    return ::basename(path);
}

#undef basename

dodo::string
filesystem::basename(const dodo::string &path)
{
    if (path.size() >= PATH_MAXLEN)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_BASENAME, exception::ERRNO_LIBDODO, FILESYSTEMEX_TOOLONGPATH, TOOLSFILESYSTEMEX_TOOLONGPATH_STR, __LINE__, __FILE__, path);

    char temp[PATH_MAXLEN];

    strcpy(temp, path.data());

    return original_basename(temp);
}
#else
dodo::string
filesystem::basename(const dodo::string &path)
{
    if (path.size() >= PATH_MAXLEN)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_BASENAME, exception::ERRNO_LIBDODO, FILESYSTEMEX_TOOLONGPATH, TOOLSFILESYSTEMEX_TOOLONGPATH_STR, __LINE__, __FILE__, path);

    char temp[PATH_MAXLEN];

    strcpy(temp, path.data());

    return ::basename(temp);
}
#endif

//-------------------------------------------------------------------

dodo::string
filesystem::dirname(const dodo::string &path)
{
    if (path.size() >= PATH_MAXLEN)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_DIRNAME, exception::ERRNO_LIBDODO, FILESYSTEMEX_TOOLONGPATH, TOOLSFILESYSTEMEX_TOOLONGPATH_STR, __LINE__, __FILE__, path);

    char temp[PATH_MAXLEN];

    strcpy(temp, path.data());

    return ::dirname(temp);
}

//-------------------------------------------------------------------

void
filesystem::copy(const dodo::string &from,
                 const dodo::string &a_to,
                 bool             force)
{
    dodo::string to = a_to;

    {
        char temp[PATH_MAXLEN];

        strcpy(temp, to.data());
        char *bname = ::basename(temp);
        if (strcmp(bname, "..") == 0  || strcmp(bname, ".") == 0 || a_to[a_to.size() - 1] == '/')
            to = bname + dodo::string("/") + basename((char *)from.data());
    }

    struct stat stFrom, stTo;

    if (::lstat(from.data(), &stFrom) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

    if (::lstat(to.data(), &stTo) == -1) {
        if (errno != ENOENT)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
    } else {
        if (force) {
            if (!S_ISDIR(stTo.st_mode)) {
                if (::unlink(to.data()) == -1)
                    dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
            } else if (::rmdir(to.data()) == -1)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_UNLINK, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
        } else
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, to);
    }

    if (!S_ISREG(stFrom.st_mode)) {
        if (S_ISDIR(stFrom.st_mode)) {
            if (::mkdir(to.data(), stFrom.st_mode) == 1)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
        } else if (S_ISLNK(stFrom.st_mode)) {
            char buffer[PATH_MAXLEN];
            int count = 0;

            if ((count = ::readlink(from.data(), buffer, PATH_MAXLEN)) == -1)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

            buffer[count] = '\0';

            if (::symlink(buffer, to.data()) == -1)
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, dodo::string(buffer) + "->" + to);
        } else if (::mknod(to.data(), stFrom.st_mode, 0) == 1)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
    } else {
        long iter = stFrom.st_size / IO_BLOCKSIZE, rest = stFrom.st_size % IO_BLOCKSIZE;

        FILE *fromFile = fopen(from.data(), "r");
        if (fromFile == NULL)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

        FILE *toFile = fopen(to.data(), "w+");
        if (toFile == NULL)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

        char buffer[IO_BLOCKSIZE];

        int i(0);
        for (; i < iter; ++i) {
            if (fread(buffer, IO_BLOCKSIZE, 1, fromFile) == 0) {
                switch (errno) {
                    case EIO:
                    case EINTR:
                    case ENOMEM:
                    case EOVERFLOW:
                    case EROFS:

                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
                }
            }

            if (fwrite(buffer, IO_BLOCKSIZE, 1, toFile) == 0) {
                switch (errno) {
                    case EIO:
                    case EINTR:
                    case ENOMEM:
                    case EOVERFLOW:
                    case EROFS:

                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
                }
            }
        }
        if (rest > 0) {
            if (fread(buffer, rest, 1, fromFile) == 0) {
                switch (errno) {
                    case EIO:
                    case EINTR:
                    case ENOMEM:
                    case EOVERFLOW:
                    case EROFS:

                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
                }
            }

            if (fwrite(buffer, rest, 1, toFile) == 0) {
                switch (errno) {
                    case EIO:
                    case EINTR:
                    case ENOMEM:
                    case EOVERFLOW:
                    case EROFS:

                        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
                }
            }
        }

        if (fclose(fromFile) != 0)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);

        if (fclose(toFile) != 0)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPY, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from + "->" + to);
    }
}

//-------------------------------------------------------------------

void
filesystem::copyDir(const dodo::string &from,
                    const dodo::string &a_to,
                    bool             force)
{
    dodo::string to = a_to;

    {
        char temp[PATH_MAXLEN];

        strcpy(temp, to.data());
        char *bname = ::basename(temp);
        if (strcmp(bname, "..") == 0 || strcmp(bname, ".") == 0 || a_to[a_to.size() - 1] == '/')
            to = bname + dodo::string("/") + basename((char *)from.data());
    }

    struct stat stFrom, stTo;

    if (::lstat(from.data(), &stFrom) == -1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);

    if (::lstat(to.data(), &stTo) == -1) {
        if (errno != ENOENT)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);
    } else if (force)
        filesystem::rm(to, force);
    else
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_LIBDODO, FILESYSTEMEX_WRONGFILENAME, TOOLSFILESYSTEMEX_WRONGFILENAME_STR, __LINE__, __FILE__, to);

    if (!S_ISDIR(stFrom.st_mode))
        filesystem::copy(from, to, force);
    else {
        if (::mkdir(to.data(), stFrom.st_mode) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, to);

        dodo::string attachedFrom, attachedTo;

        DIR *directory = opendir(from.data());
        if (directory == NULL) {
            if (errno == ENOENT)
                return ;
            else
                dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
        }

        dirent *dd;

        while ((dd = readdir(directory)) != NULL) {
            if ((strcmp(dd->d_name, ".") == 0) || (strcmp(dd->d_name, "..") == 0))
                continue;

            attachedTo = dodo::string(to + "/" + dd->d_name);
            attachedFrom = dodo::string(from + "/" + dd->d_name);

            filesystem::copyDir(attachedFrom, attachedTo, force);
        }

        if (closedir(directory) == -1)
            dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX_COPYDIR, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, from);
    }
}

//-------------------------------------------------------------------

bool
filesystem::exists(const dodo::string &path)
{
    struct stat st;

    if (::lstat(path.data(), &st) == 0)
        return true;

    return false;
}

//-------------------------------------------------------------------

void
filesystem::appendToFile(const dodo::string &path,
                         const dodo::string &content)
{
    _writeToFile(path, content, "a+");
}

//-------------------------------------------------------------------

void
filesystem::appendToFile(const dodo::string      &path,
                         const dodoStringArray &content)
{
    _writeToFile(path, content, "a+");
}

//-------------------------------------------------------------------

void
filesystem::writeToFile(const dodo::string &path,
                        const dodo::string &content)
{
    _writeToFile(path, content, "w+");
}

//-------------------------------------------------------------------

void
filesystem::writeToFile(const dodo::string      &path,
                        const dodoStringArray &content)
{
    _writeToFile(path, content, "w+");
}

//-------------------------------------------------------------------

void
filesystem::_writeToFile(const dodo::string &path,
                         const dodo::string &content,
                         const char       *mode)
{
    FILE *file = fopen(path.data(), mode);
    if (file == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    unsigned long size = content.size();
    if (size > 0 && fwrite(content.data(), size, 1, file) != 1)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
}

//-------------------------------------------------------------------

void
filesystem::_writeToFile(const dodo::string      &path,
                         const dodoStringArray &content,
                         const char            *mode)
{
    FILE *file = fopen(path.data(), mode);
    if (file == NULL)
        dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);

    dodoStringArray::const_iterator i = content.begin(), j = content.end();
    for (; i != j; ++i) {
        if (i->size() > 0 && fputs(i->data(), file) < 0) {
            switch (errno) {
                case EFBIG:
                case EIO:
                case EINTR:
                case EBADF:
                case EOVERFLOW:
                case ENOSPC:
                case EPIPE:
                case ENOMEM:
                case ENXIO:

                    dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
            }
        }

        if (fputc('\n', file) < 0) {
            switch (errno) {
                case EFBIG:
                case EIO:
                case EINTR:
                case EBADF:
                case EOVERFLOW:
                case ENOSPC:
                case EPIPE:
                case ENOMEM:
                case ENXIO:

                    dodo_throw exception::basic(exception::MODULE_TOOLSFILESYSTEM, FILESYSTEMEX__WRITETOFILE, exception::ERRNO_ERRNO, errno, strerror(errno), __LINE__, __FILE__, path);
            }
        }
    }
}

//-------------------------------------------------------------------
