CC=g++

all: colorize

colorize: colorize.o Color.h.gch ArgsX.h.gch
	$(CC) colorize.o -o colorize

colorize.o: colorize.cpp
	$(CC) -c colorize.cpp

Color.h.gch: Color.h
	$(CC) -c Color.h -o Color.o

ArgsX.h.gch: ArgsX.h
	$(CC) -c ArgsX.h -o ArgsX.o

clean:
	rm *o colorize

install:
	cp colorize /bin/
remove:
	rm -f  /bin/colorize