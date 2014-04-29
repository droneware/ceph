#!/usr/bin/bash
cd ..
export PATH=/usr/vacpp/bin:$PATH
export CC=xlc_r
export CXX=xlc++_r

export CEPH_DEV_HOME=/home/inktank

export CFLAGS="-g"
export OBJECT_MODE=64

export LDFLAGS="-L/opt/freeware/lib -L$CEPH_DEV_HOME/boost_1_55_0/stage/lib"
export CPPFLAGS="-I/opt/freeware/include -I$CEPH_DEV_HOME/boost_1_55_0 -I$CEPH_DEV_HOME/boost_1_55_0/boost/tr1"

./configure --without-fuse --without-libatomic-ops --without-libaio --without-tcmalloc --with-nss --without-libedit --without-snappy --without-leveldb --without-libxfs --with-debug

#
# cryptopp
#
#export LDFLAGS="-L/opt/freeware/lib -L$CEPH_DEV_HOME/boost_1_55_0/stage/lib -L$CEPH_DEV_HOME/cryptopp-562 -lcryptopp "
#export CPPFLAGS="-I/opt/freeware/include -I$CEPH_DEV_HOME/boost_1_55_0 -I$CEPH_DEV_HOME/boost_1_55_0/boost/tr1 -I$CEPH_DEV_HOME/cryptopp-562"
#./configure --without-fuse --without-libatomic-ops --without-libaio --without-tcmalloc --with-cryptopp --with-debug  --without-libedit
