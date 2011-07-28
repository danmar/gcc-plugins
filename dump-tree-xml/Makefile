# build dump-tree-xml plugin

# path to gcc source code
GCC_SRC = ~/gcc

# path to gcc build
GCC_BUILD = ~/gcc/build

INCLUDE  = -I../include \
    -I $(GCC_SRC)/gcc \
    -I $(GCC_BUILD)/gcc \
    -I $(GCC_SRC)/include

NAME = dump-tree-xml

$(NAME).so:	$(NAME).c	../include/parse-tree.h
	gcc $(INCLUDE) -shared -fPIC -o $(NAME).so $(NAME).c

test1:	$(NAME).so
	$(GCC_BUILD)/gcc/cc1 -fplugin=$(CURDIR)/$(NAME).so -fsyntax-only test1.c

