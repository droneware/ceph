/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2011 Stanislav Sedov <stas@FreeBSD.org>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 */

#ifndef CEPH_COMPAT_H
#define CEPH_COMPAT_H

#if defined(__FreeBSD__)
#define	ENODATA	61
#define	MSG_MORE 0
#endif /* !__FreeBSD__ */

#ifndef TEMP_FAILURE_RETRY
#if defined(GCC) || defined(CLANG)
#define TEMP_FAILURE_RETRY(expression) ({     \
  typeof(expression) __result;                \
  do {                                        \
    __result = (expression);                  \
  } while (__result == -1 && errno == EINTR); \
  __result; })
#endif
#if defined(XLC)
#define TEMP_FAILURE_RETRY(expression) ({     \
  expression; })
#endif
#endif

#ifdef __cplusplus
# define VOID_TEMP_FAILURE_RETRY(expression) \
   static_cast<void>(TEMP_FAILURE_RETRY(expression))
#else
# define VOID_TEMP_FAILURE_RETRY(expression) \
   do { (void)TEMP_FAILURE_RETRY(expression); } while (0)
#endif

#if defined(__FreeBSD__) || defined(__APPLE__)
#define lseek64(fd, offset, whence) lseek(fd, offset, whence)
#endif

#if defined(XLC)
#define MSG_DONTWAIT MSG_NONBLOCK
#define LOG_AUTHPRIV    (10<<3)
#define LOG_FTP         (11<<3)
#define SPLICE_F_NONBLOCK (0x02)
#define __STRING(x)     "x"
//required for included mds header, defined here for librados compilation only 
#define IFTODT(mode)   (((mode) & 0170000) >> 12)
#endif

#endif /* !CEPH_COMPAT_H */
