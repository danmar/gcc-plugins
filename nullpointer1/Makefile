
# gcc is checked out to ~/gcc
# the build dir is gcc/build

INCLUDE  = -I ../include \
    -I ~/gcc/gcc \
    -I ~/gcc/build/gcc \
    -I ~/gcc/build/gmp \
    -I ~/gcc/include \
    -I ~/gcc/libcpp/include

nullpointer1.so:	nullpointer1.c	../include/parse-tree.h
	gcc $(INCLUDE) -shared -fPIC -o nullpointer1.so nullpointer1.c


