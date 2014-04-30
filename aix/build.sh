#!/opt/freeware/bin/bash

rm -rf pkg-build
mkdir -p pkg-build/usr/lib
mkdir -p pkg-build/usr/bin
mkdir -p pkg-build/usr/include/rados

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

mkinstallp -d pkg-build -T librados.template
