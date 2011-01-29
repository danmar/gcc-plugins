
# gcc is checked out to ~/gcc
# the build dir is gcc/build

INCLUDE  = -I ~/gcc/gcc \
    -I ~/gcc/build/gcc \
    -I ~/gcc/include

hello.so:	hello.c
	gcc $(INCLUDE) -shared -fPIC -o hello.so hello.c


