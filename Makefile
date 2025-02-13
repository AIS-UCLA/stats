ifeq ($(shell uname -s),Linux)
	DESTDIR=/usr/
	BINDIR= local/bin
	MANDIR= share/man/
	else
	DESTDIR=/usr/local/
	BINDIR= bin
	MANDIR:=share/man/
endif

BINOWN= root
BINMODE=0544

CFLAGS+=  -W -Wextra -Wpedantic -Werror \
          -std=c89 \
          -fPIE
LIBS=     -lrrd
LDFLAGS+=

cpu.o: cpu.c
	$(CC) -c -o $@ $< $(CFLAGS)

cpu: cpu.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

install-cpu: cpu
	install -o $(BINOWN) -m $(BINMODE) -s $^ $(DESTDIR)$(BINDIR)/submit-$^

