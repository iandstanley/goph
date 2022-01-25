# Makefile to build gopherd
MANPAGE= docs/*.mdoc
HEADERFILES= server.h protocol.h main.h client.h
SRC= main.c server.c
PRETTY= indent -cdb -ut
BIN=gopherd
BIN=a.out
CCOPTS=-g -ferror-limit=4

$(BIN): $(SRC)	 $(HEADERFILES)
	cc $(CCOPTS) $(SRC) -o $@

man:	$(MANPAGE)
	mandoc docs/gopherd.mdoc | pg

pretty:
	$(PRETTY) main.c
	$(PRETTY) protocol.h
	$(PRETTY) main.h
	$(PRETTY) server.h
	$(PRETTY) server.c
	$(PRETTY) client.h

tags:
	ctags -d *.c *.h

clean:
	-rm *.o 
	-rm *.BAK
	-rm $(BIN)
	-rm *.core

test:

debug:
	gdb -x gdb.script   $(BIN) -p 7070 

