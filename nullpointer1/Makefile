
# gcc is checked out to ~/gcc
# the build dir is gcc/build

INCLUDE  = -I ~/gcc/gcc \
    -I ~/gcc/build/gcc \
    -I ~/gcc/build/gmp \
    -I ~/gcc/include \
    -I ~/gcc/libcpp/include

nullpointer1.so:	nullpointer1.c
	gcc $(INCLUDE) -shared -fPIC -o nullpointer1.so nullpointer1.c


