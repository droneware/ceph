#!/opt/freeware/bin/bash

# Make sure only root can run our script
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
#   exit 1
fi

rm -rf pkg-build
mkdir -p pkg-build/usr/lib
mkdir -p pkg-build/usr/bin
mkdir -p pkg-build/usr/include/rados
mkdir -p pkg-build/usr/share/man/man8
mkdir -p pkg-build/usr/share/doc/librados-devel
mkdir -p pkg-build/usr/share/doc/librados-rte

cp ../src/.libs/librados.a pkg-build/usr/lib/.
cp ../src/.libs/librados.so.2 pkg-build/usr/lib/.
cp ../src/.libs/librados.la pkg-build/usr/lib/.

cp ../src/.libs/librados-config pkg-build/usr/bin/.
cp ../src/include/rados/buffer.h pkg-build/usr/include/rados
cp ../src/include/rados/crc32c.h pkg-build/usr/include/rados
cp ../src/include/rados/librados.h pkg-build/usr/include/rados
cp ../src/include/rados/librados.hpp pkg-build/usr/include/rados
cp ../src/include/rados/page.h pkg-build/usr/include/rados
cp ../src/include/rados/rados_types.h pkg-build/usr/include/rados
cp ../src/include/rados/rados_types.hpp pkg-build/usr/include/rados

cp ../man/librados-config.8 pkg-build/usr/share/man/man8
cp ../COPYING pkg-build/usr/share/doc/librados-devel/copyright
cp ../COPYING pkg-build/usr/share/doc/librados-rte/copyright

mkinstallp -d pkg-build -T librados.template
