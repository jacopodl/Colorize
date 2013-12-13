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
remove:
	rm -f  /bin/colorize
