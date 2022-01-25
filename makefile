# Makefile to build gopherd
MANPAGE= docs/*.mdoc
PRETTY= indent -cdb -ut
BINS=a.out testsuite
CC=clang $(CFLAGS)
CFLAGS=-g -ferror-limit=1
LIBSRC=gopher.c error.c
LIBOBJ=gopher.o error.o
APPSRC=main.c 
TESTSRC=test.c 
TESTBIN=test_suite

DEFAULT: test

app:	a.out

a.out: 	library main.c 
	$(CC) main.c -DNDEBUG  -L. -lgopher

############################################################

test:	testsuite
	./testsuite

testsuite:	libgopher.a tests.o
	$(CC) tests.o -DDEBUG -L. -lgopher  
	mv a.out testsuite
	sleep 2

tests.o:	tests.c
	$(CC) -c $> -o $@


############################################################

library: 	libgopher.a 

libgopher.a: 	$(LIBOBJ)
	$(CC) -c $(LIBSRC) 
	ar rcs libgopher.a $(LIBOBJ) 
	echo libgopher.a created


# cc -c libadd.c -o libadd.o
# cc -c libsub.c -o libsub.o
# ar rcs libcalc.a libadd.o libsub.o
# cc main.c -o main.o
# cc -o main main.o -L. libcalc

############################################################

tags:
	ctags -d *.c *.h

clean:
	-rm *.BAK
	-rm *.o
	-rm *.core
	-rm $(TESTBIN)
	-rm $(BINS)
	-rm a.out
	-rm libgopher.a
	clear
	@ls

debug:
	gdb -x gdb.script   $(BIN) -p 7070 

pretty:	$(ALLSRC)
	$(PRETTY) *.c

man:	$(MANPAGE)
	mandoc docs/gopherd.mdoc | pg
