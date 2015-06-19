CC=gcc

all: colorize

colorize: colorize.o argsx.o
	$(CC) colorize.o argsx.o -o colorize

colorize.o: colorize.c
	$(CC) -c colorize.c

argsx.o: argsx.c
	$(CC) -c argsx.c

clean:
	rm *o colorize

install:
	cp colorize /bin/
	install -g 0 -o 0 -m 0644 colorize.8 /usr/share/man/man8/
	gzip /usr/share/man/man8/colorize.8
remove:
	rm -v -f /bin/colorize /usr/share/man/man8/colorize.8.gz
