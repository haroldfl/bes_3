##
## @file Makefile
## Betriebssysteme sender/empfänger Makefile
## Beispiel 3
##
## @author ibrahim Milli <ic17b063@technikum-wien.at>
## @author Dominic Ernstbrunner <ic17b015@technikum-wien.at>
## @author Florian Harold <ic17b093@technikum-wien.at>
os
CC=gcc52
CFLAGS=-Wall -Werror -Wextra -Wstrict-prototypes -Wformat=2 -pedantic -fno-common -ftrapv -O3 -g -std=gnu11
LIBS = -lpthread -lrt


DOXYGEN=doxygen
CD=cd
MV=mv
RM=rm
GREP=grep
EXCLUDE_PATTERN=footrulewidth


OBJECTS_1=receiver.o
OBJECTS_2=sender.o
OBJECTS_3=send_rec.o
HEADER=send_rec.h

%.o: %.c
	$(CC) $(CFLAGS)  -c $<


all: receiver sender

receiver: $(OBJECTS_1) $(OBJECTS_3)
	$(CC) $(CFLAGS) $(OBJECTS_1) $(OBJECTS_3) $(HEADER) -o$@ $(LIBS)

sender: $(OBJECTS_2) $(OBJECTS_3)
	$(CC) $(CFLAGS) $(OBJECTS_2) $(OBJECTS_3) $(HEADER) -o$@ $(LIBS)


.PHONY: clean

clean: 
	$(RM) -f *.o
	$(RM) sender
	$(RM) receiver



.PHONY: distclean

distclean: clean
	$(RM) -rf doc

doc: html pdf

.PHONY: html

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

