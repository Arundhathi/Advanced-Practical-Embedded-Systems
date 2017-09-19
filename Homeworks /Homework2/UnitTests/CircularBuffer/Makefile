CC = cc
CFLAGS = -std=c99 -Wall -Werror -g -O0

#
# build third-party libraries
#
SUBDIRS = 3rd-party

ifdef SUBDIRS
.PHONY : $(SUBDIRS)
$(SUBDIRS) : 
	@if [ -d $@ ]; then \
		$(MAKE) --no-print-directory --directory=$@ \
			CC=$(CC) CFLAGS="$(CFLAGS)" $(MAKECMDGOALS); \
	fi	
endif


THIRD_PARTY_DIR = 3rd-party
CMOCKA_INCLUDE_DIR = $(THIRD_PARTY_DIR)/build-Debug/include
CMOCKA_LIBRARY = $(THIRD_PARTY_DIR)/build-Debug/lib/libcmocka.a

cmocka : $(SUBDIRS)

CFLAGS += -I $(CMOCKA_INCLUDE_DIR)

#
# quick and dirty build rules to demonstrate testing, not good build
# system practices.
#

#
# library
#
LIB_SRCS = \
	circularbuffer.c#name of source file to be added 

LIB_OBJS = $(LIB_SRCS:%.c=%.o)

LIB_HEADERS = $(LIB_SRCS:%.c=%.h)

LIB = libutils.a

#
# main executable
#
EXE_SRCS = main.c #name of main file with main() function to be added
EXE_OBJS = $(EXE_SRCS:%.c=%.o)

EXE = main.out #name of mains executable that will be created 

#
# tests
#
TEST_QUADRATIC_SRCS = \
	uttestCB.c #name of the source file containing unit tests

TEST_QUADRATIC_OBJS = $(TEST_QUADRATIC_SRCS:%.c=%.o)

TEST_QUADRATIC_EXE = test-CB.out#name of executable of unit tests source file 


#
# Generic rule to generate objects
#
%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LIB) : $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $^

all : $(LIB) $(EXE)

$(EXE) : $(EXE_OBJS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ -lm

test : $(TEST_QUADRATIC_EXE)
	./$(TEST_QUADRATIC_EXE)

$(TEST_QUADRATIC_EXE) : $(TEST_QUADRATIC_SRCS) $(LIB)
	$(CC) $(CFLAGS) -o $@ $^ $(CMOCKA_LIBRARY) -lm


.PHONY : clean
clean : FORCE
	-rm -rf *~ *.o $(LIB) $(EXE) $(TEST_QUADRATIC_EXE) *.dSYM/

.PHONY : clobber
clobber : $(SUBDIRS) clean


FORCE :
