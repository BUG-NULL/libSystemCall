//
//  z_dirent.c
//  SystemCall
//
//  Created by NULL
//

#include "z_dirent.h"
#include "z_syscall.h"
#include "z_error.h"
#include "z_string.h"
#include "z_fcntl.h"
#include "z_unistd.h"
#include "z_malloc.h"
#include "z_sysctl.h"
#include "z_mount.h"
#include "z_stdlib.h"

z_int8 z_kernel_supports_unionfs(z_void)
{
    z_int8 kernel_supports_unionfs = -1;
    if (kernel_supports_unionfs == -1)
    {
        z_int32 value = 0;
        z_int32 len = sizeof(value);
        z_sysctlbyname("kern.secure_kernel", &value, &len, Z_NULL, 0);
        kernel_supports_unionfs = !value;
    }
    return kernel_supports_unionfs;
}

z_int32 z_fd_is_on_union_mount(z_int32 fd)
{
    struct z_statfs64 stbuf;
    z_int32 rc;

    rc = z_fstatfs(fd, &stbuf);
    if (rc < 0)
    {
        return rc;
    }
    return (stbuf.f_flags & MNT_UNION) != 0;
}

z_int32 z_opendir_compar(z_void *p1, z_void *p2)
{
    return (z_strncmp((*(struct z_dirent **)p1)->d_name, (*(struct z_dirent **)p2)->d_name, MAXPATHLEN));
}

z_int8 z_filldir(struct z_dir *dirp, z_int8 use_current_pos)
{
    struct z_dirent **dpv;
    z_int8 *buf, *ddptr, *ddeptr;
    z_int64 pos;
    z_int32 fd2, incr, len, n, saved_errno, space;
    
    len = 0;
    space = 0;
    buf = Z_NULL;
    ddptr = Z_NULL;

    /*
     * Use the system page size if that is a multiple of DIRBLKSIZ.
     * Hopefully this can be a big win someday by allowing page
     * trades to user space to be done by _getdirentries().
     */
    incr = z_getpagesize();
    if ((incr % DIRBLKSIZ) != 0)
        incr = DIRBLKSIZ;

    /*
     * The strategy here is to read all the directory
     * entries into a buffer, sort the buffer, and
     * remove duplicate entries by setting the inode
     * number to zero.
     *
     * We reopen the directory because _getdirentries()
     * on a MNT_UNION mount modifies the open directory,
     * making it refer to the lower directory after the
     * upper directory's entries are exhausted.
     * This would otherwise break software that uses
     * the directory descriptor for fchdir or *at
     * functions, such as fts.c.
     */
    if ((fd2 = z_openat(dirp->__dd_fd, ".", O_RDONLY | O_CLOEXEC)) == -1)
        return 0;

    if (use_current_pos)
    {
        pos = z_lseek(dirp->__dd_fd, 0, SEEK_CUR);
        if (pos == -1 || z_lseek(fd2, pos, SEEK_SET) == -1)
        {
            saved_errno = (z_int32)z_errno;
            z_close(fd2);
            z_errno = saved_errno;
            return 0;
        }
    }

    do {
        /*
         * Always make at least DIRBLKSIZ bytes
         * available to _getdirentries
         */
        if (space < DIRBLKSIZ)
        {
            space += incr;
            len += incr;
//            buf = z_reallocf(buf, len);
            if (buf == Z_NULL)
            {
                saved_errno = (z_int32)z_errno;
                z_close(fd2);
                z_errno = saved_errno;
                return 0;
            }
            ddptr = buf + (len - space);
        }

        n = (z_int32)z_getdirentries(fd2, ddptr, space, &dirp->__dd_td->seekoff);
        if (n > 0)
        {
            ddptr += n;
            space -= n;
        }
        if (n < 0)
        {
            saved_errno = (z_int32)z_errno;
            z_close(fd2);
            z_errno = saved_errno;
            return 0;
        }
    } while (n > 0);
    z_close(fd2);

    ddeptr = ddptr;

    /*
     * There is now a buffer full of (possibly) duplicate
     * names.
     */
    dirp->__dd_buf = buf;

    /*
     * Go round this loop twice...
     *
     * Scan through the buffer, counting entries.
     * On the second pass, save pointers to each one.
     * Then sort the pointers and remove duplicate names.
     */
    for (dpv = Z_NULL;;)
    {
        n = 0;
        ddptr = buf;
        while (ddptr < ddeptr)
        {
            struct z_dirent *dp;

            dp = (struct z_dirent *) ddptr;
            if ((z_int64)dp & 03L)
                break;
            if ((dp->d_reclen <= 0) || (dp->d_reclen > (ddeptr + 1 - ddptr)))
                break;
            ddptr += dp->d_reclen;
            if (dp->d_ino)
            {
                if (dpv)
                    dpv[n] = dp;
                n++;
            }
        }

        if (dpv)
        {
            struct z_dirent *xp;

            /*
             * This sort must be stable.
             */
            z_mergesort(dpv, n, sizeof(*dpv), z_opendir_compar);

            dpv[n] = Z_NULL;
            xp = Z_NULL;

            /*
             * Scan through the buffer in sort order,
             * zapping the inode number of any
             * duplicate names.
             */
            for (n = 0; dpv[n]; n++)
            {
                struct z_dirent *dp = dpv[n];

                if ((xp == Z_NULL) || z_strncmp(dp->d_name, xp->d_name, MAXPATHLEN))
                {
                    xp = dp;
                }
                else
                {
                    dp->d_ino = 0;
                }
                if (dp->d_type == DT_WHT && (dirp->__dd_flags & DTF_HIDEW))
                    dp->d_ino = 0;
            }

            z_free(dpv, 0);
            break;
        }
        else
        {
            dpv = z_malloc((n + 1) * sizeof(struct z_dirent *));
            if (dpv == Z_NULL)
                break;
        }
    }

    dirp->__dd_len = len;
    dirp->__dd_size = ddptr - dirp->__dd_buf;
    return 1;
}

struct z_dir *z_opendir_common(z_int32 fd, z_int32 flags, z_int8 use_current_pos)
{
    struct z_dir *dirp;
    z_int32 saved_errno;
    z_int32 unionstack;

    if ((dirp = z_malloc(sizeof(struct z_dir) + sizeof(struct z_telldir))) == Z_NULL)
        return Z_NULL;

    dirp->__dd_buf = Z_NULL;
    dirp->__dd_fd = fd;
    dirp->__dd_flags = flags;
    dirp->__dd_loc = 0;
//    dirp->__dd_lock = {0x32AAABA7, {0}};
    dirp->__dd_td = (struct z_telldir *)((z_int8 *)dirp + sizeof(struct z_dir));
    dirp->__dd_td->td_locq.lh_first = Z_NULL;
    dirp->__dd_td->td_loccnt = 0;

    /*
     * Determine whether this directory is the top of a union stack.
     */
    if ((flags & DTF_NODUP) && z_kernel_supports_unionfs())
    {
        unionstack = z_fd_is_on_union_mount(fd);
        if (unionstack < 0)
            goto fail;
    }
    else
    {
        unionstack = 0;
    }

    if (unionstack)
    {
        if (!z_filldir(dirp, use_current_pos))
            goto fail;
        dirp->__dd_flags |= __DTF_READALL;
    }
    else
    {
        /*
         * Start with a small-ish size to avoid allocating full pages.
         * readdir() will allocate a larger buffer if it didn't fit
         * to stay fast for large directories.
         */
        dirp->__dd_len = READDIR_INITIAL_SIZE;
        dirp->__dd_buf = z_malloc(dirp->__dd_len);
        if (dirp->__dd_buf == Z_NULL)
            goto fail;
        if (use_current_pos)
        {
            /*
             * Read the first batch of directory entries
             * to prime dd_seek.  This also checks if the
             * fd passed to fdopendir() is a directory.
             */
            /*
             * sufficiently recent kernels when the buffer is large enough,
             * will use the last bytes of the buffer to return status.
             *
             * To support older kernels:
             * - make sure it's 0 initialized
             * - make sure it's past `dd_size` before reading it
             */
            enum z_getdirentries64_flags_t *gdeflags = (enum z_getdirentries64_flags_t *)(dirp->__dd_buf + dirp->__dd_len - sizeof(enum z_getdirentries64_flags_t));
            *gdeflags = 0;
            dirp->__dd_size = (z_int64)z_getdirentries(dirp->__dd_fd, dirp->__dd_buf, dirp->__dd_len, &dirp->__dd_td->seekoff);
            if (dirp->__dd_size >= 0 && dirp->__dd_size <= dirp->__dd_len - sizeof(enum z_getdirentries64_flags_t))
            {
                if (*gdeflags & GETDIRENTRIES64_EOF)
                {
                    dirp->__dd_flags |= __DTF_ATEND;
                }
            }
            if (dirp->__dd_size < 0)
            {
                if (z_errno == EINVAL)
                    z_errno = ENOTDIR;
                goto fail;
            }
            dirp->__dd_flags |= __DTF_SKIPREAD;
        }
        else
        {
            dirp->__dd_size = 0;
            dirp->__dd_td->seekoff = 0;
        }
    }

    return dirp;
fail:
    saved_errno = (z_int32)z_errno;
    z_free(dirp->__dd_buf, 0);
    z_free(dirp, 0);
    z_errno = saved_errno;
    return Z_NULL;
}

struct z_dir *z_opendir2(z_int8 *name, z_int32 flags)
{
    z_int32 fd;
    struct z_dir *dir;
    z_int32 saved_errno;

    if ((flags & (__DTF_READALL | __DTF_SKIPREAD)) != 0)
        return Z_NULL;
    
    if ((fd = z_open(name, O_RDONLY | O_NONBLOCK | O_DIRECTORY | O_CLOEXEC, 0)) == -1)
        return Z_NULL;

    dir = z_opendir_common(fd, flags, 0);
    if (dir == Z_NULL)
    {
        saved_errno = (z_int32)z_errno;
        z_close(fd);
        z_errno = saved_errno;
    }
    return dir;
}

struct z_dir *z_opendir(z_int8 *filename)
{
    return z_opendir2(filename, DTF_HIDEW | DTF_NODUP);
}

z_int32 z_getdirentries(z_int32 fd, z_int8 *buf, z_int32 nbytes, z_int64 *basep)
{
    return (z_int32)(z_syscall_unix(SYSCALL_UNIX(SYS_getdirentries64), fd, buf, nbytes, basep));
}

z_void z_reclaim_telldir(struct z_dir *dirp)
{
    struct z_ddloc *lp;
    struct z_ddloc *templp;

    lp = dirp->__dd_td->td_locq.lh_first;
    while (lp != Z_NULL)
    {
        templp = lp;
        lp = lp->loc_lqe.le_next;
        z_free(templp, 0);
    }
    dirp->__dd_td->td_locq.lh_first = Z_NULL;
}

z_int32 z_fdclosedir(struct z_dir *dirp)
{
    z_int32 fd;

    fd = dirp->__dd_fd;
    dirp->__dd_fd = -1;
    dirp->__dd_loc = 0;
    z_free((z_void *)dirp->__dd_buf, 0);
    z_reclaim_telldir(dirp);
    z_free((void *)dirp, 0);
    return (fd);
}

z_int32 z_closedir(struct z_dir *dirp)
{
    return (z_close(z_fdclosedir(dirp)));
}

z_void z_fixtelldir(struct z_dir *dirp, z_int64 oldseek, z_int64 oldloc)
{
    struct z_ddloc *lp;

    lp = dirp->__dd_td->td_locq.lh_first;
    if (lp != Z_NULL)
    {
        if (lp->loc_loc == oldloc && lp->loc_seek == oldseek)
        {
            lp->loc_seek = dirp->__dd_td->seekoff;
            lp->loc_loc = dirp->__dd_loc;
        }
    }
}

struct z_dirent *z_readdir(struct z_dir *dirp)
{
    struct z_dirent *dp;
    z_int64 initial_seek;
    z_int64 initial_loc = 0;

    for (;;)
    {
        if (dirp->__dd_loc >= dirp->__dd_size)
        {
            if (dirp->__dd_flags & (__DTF_READALL | __DTF_ATEND))
                return (Z_NULL);
            initial_loc = dirp->__dd_loc;
            dirp->__dd_flags &= ~__DTF_SKIPREAD;
            dirp->__dd_loc = 0;
        }
        if (dirp->__dd_loc == 0
            && !(dirp->__dd_flags & (__DTF_READALL | __DTF_ATEND | __DTF_SKIPREAD)))
        {
            if (dirp->__dd_len == READDIR_INITIAL_SIZE)
            {
                /*
                 * If we need to read more, and we still have the original size,
                 * then grow the internal buffer to a large size to amortize
                 * the cost of __getdirentries64 calls.
                 */
                z_int32 len = READDIR_LARGE_SIZE;
                z_int8 *buf = z_malloc(len);
                if (buf)
                {
                    z_free(dirp->__dd_buf, 0);
                    dirp->__dd_buf = buf;
                    dirp->__dd_len = len;
                }
            }
            /*
             * sufficiently recent kernels when the buffer is large enough,
             * will use the last bytes of the buffer to return status.
             *
             * To support older kernels:
             * - make sure it's 0 initialized
             * - make sure it's past `dd_size` before reading it
             */
            enum z_getdirentries64_flags_t *gdeflags =
                (enum z_getdirentries64_flags_t *)(dirp->__dd_buf + dirp->__dd_len -
                sizeof(enum z_getdirentries64_flags_t));
            *gdeflags = 0;
            initial_seek = dirp->__dd_td->seekoff;
            dirp->__dd_size = (z_int64)z_getdirentries(dirp->__dd_fd, dirp->__dd_buf, dirp->__dd_len, &dirp->__dd_td->seekoff);
            if (dirp->__dd_size >= 0
                && dirp->__dd_size <= dirp->__dd_len - sizeof(enum z_getdirentries64_flags_t))
            {
                if (*gdeflags & GETDIRENTRIES64_EOF)
                {
                    dirp->__dd_flags |= __DTF_ATEND;
                }
            }
            if (dirp->__dd_size <= 0)
                return (Z_NULL);
            z_fixtelldir(dirp, initial_seek, initial_loc);
        }
        dirp->__dd_flags &= ~__DTF_SKIPREAD;
        dp = (struct z_dirent *)(dirp->__dd_buf + dirp->__dd_loc);
        if ((z_int64)dp & 03L)    /* bogus pointer check */
            return (Z_NULL);
        if (dp->d_reclen <= 0 || dp->d_reclen > dirp->__dd_len + 1 - dirp->__dd_loc)
            return (Z_NULL);
        dirp->__dd_loc += dp->d_reclen;
        if (dp->d_ino == 0)
            continue;
        if (dp->d_type == DT_WHT && (dirp->__dd_flags & DTF_HIDEW))
            continue;
        return (dp);
    }
}
