CC := ./9c
LD := ./9l
all: builtin.c chrish.c command.c
	PLAN9=./ ./9c -D _POSIX_SOURCE builtin.c chrish.c command.c 
	PLAN9=./ ./9l -o o.chrish builtin.o chrish.o command.o

clean:
	rm -f o.* *.o

vendor:
	mkdir -p lib include
	cp $(PLAN9)/lib/*.a lib/
	cp $(PLAN9)/include/*.h include/
	cp $(PLAN9)/bin/9c ./
	cp $(PLAN9)/bin/9l ./

nuke:
	rm -rf ./lib ./include ./9c ./9l
