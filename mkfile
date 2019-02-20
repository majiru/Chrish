</$objtype/mkfile
BIN=/$objtype/bin
CFLAGS=$CFLAGS -D _PLAN9_SOURCE
TARG=chrish
OFILES=\
	chrish.$O \
	command.$O \
	builtin.$O \

HFILES=chrish.h

</sys/src/cmd/mkone