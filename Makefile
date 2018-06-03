CC=gcc
CFLAGS=-DDEBUG -Wall -Werror -pedantic -Wextra -Wstrict-prototypes -Wformat=2 -fno-common -ftrapv -g -O3 -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen
RM=rm 

OBJECTS=send_rec.c sender.c receiver.c
OBJECTS_SEND=send_rec.c sender.c
OBJECTS_REC=send_rec.c receiver.c

EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##

%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##


all: send_rec sender receiver

# make send_rec to a library
send_rec: $(OBJECTS)
	$(CC) -c -Wall -Werror -fpic send_rec.c
	$(CC) -shared -o libsend_rec.a send_rec.o

sender: $(OBJECTS)
	$(CC) -L. $(CFLAGS) -o sender $(OBJECTS_SEND) -lpthread -lrt -lsend_rec

receiver: $(OBJECTS)
	$(CC) -L. $(CFLAGS) -o receiver $(OBJECTS_REC) -lpthread -lrt -lsend_rec

.PRECIOUS: %.tex


clean:
	$(RM) *.o *~ myfind


distclean: myfind
	$(RM) -r doc

doc: html pdf


html:
	$(DOXYGEN) doxygen.dcf

pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf

