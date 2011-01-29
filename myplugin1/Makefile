
# gcc is checked out to ~/gcc
# the build dir is gcc/build

INCLUDE  = -I ~/gcc/gcc \
    -I ~/gcc/build/gcc \
    -I ~/gcc/build/gmp \
    -I ~/gcc/include \
    -I ~/gcc/libcpp/include

myplugin1.so:	myplugin1.c
	gcc $(INCLUDE) -shared -fPIC -o myplugin1.so myplugin1.c


