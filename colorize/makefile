CC=g++

all: colorize

colorize: colorize.o 
	$(CC) colorize.o -o colorize

colorize.o: colorize.cpp
	$(CC) -c colorize.cpp

clean:
	rm *o colorize

install:
	cp colorize /bin/
	install -g 0 -o 0 -m 0644 colorize.8 /usr/share/man/man8/
	gzip /usr/share/man/man8/colorize.8
remove:
	rm -v -f /bin/colorize /usr/share/man/man8/colorize.8.gz
