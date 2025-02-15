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
          -std=c99 \
          -fPIE
LIBS=     -lrrd
LDFLAGS+=

cpu.o: cpu.c
	$(CC) -c -o $@ $< $(CFLAGS)

cpu: cpu.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

mem.o: mem.c
	$(CC) -c -o $@ $< $(CFLAGS)

mem: mem.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

gpu.o: gpu.c
	$(CC) -c -o $@ $< $(CFLAGS)

gpu: gpu.o
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) -lnvidia-ml

install-cpu: cpu
	install -o $(BINOWN) -m $(BINMODE) -s $^ $(DESTDIR)$(BINDIR)/submit-$^

install-mem: mem
	install -o $(BINOWN) -m $(BINMODE) -s $^ $(DESTDIR)$(BINDIR)/submit-$^

install-gpu: gpu
	install -o $(BINOWN) -m $(BINMODE) -s $^ $(DESTDIR)$(BINDIR)/submit-$^

