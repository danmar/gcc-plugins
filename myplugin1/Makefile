
# gcc is checked out to ~/gcc
# the build dir is gcc/build

INCLUDE  = -I../include \
    -I ~/gcc/gcc \
    -I ~/gcc/build/gcc \
    -I ~/gcc/build/gmp \
    -I ~/gcc/include \
    -I ~/gcc/libcpp/include

myplugin1.so:	myplugin1.c	../include/parse-tree.h
	gcc $(INCLUDE) -shared -fPIC -o myplugin1.so myplugin1.c

test:	myplugin1.so
	xgcc $(INCLUDE) -I ~/gcc/build/gcc/include -fplugin=./myplugin1.so -c myplugin1.c
