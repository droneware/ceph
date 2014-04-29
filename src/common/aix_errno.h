#ifndef CEPH_AIX_ERRNO_H
#define CEPH_AIX_ERRNO_H

void translate_aix_errno(CephContext *cct, Message *m);

#endif
