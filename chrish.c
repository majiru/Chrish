#include <u.h>
#include <libc.h>

#include "chrish.h"

void
usage(void)
{
	fprint(2, "Usage: %s [-p prompt]\n", argv0);
	exits("usage");
}

void
main(int argc, char *argv[])
{
	char in;
	char *prompt = "cpre308>";

	char buf[MAXPATH];
	char *args[MAXARG];
	int argcount;
	int offset = 0;

	int n;
	Command *c;

	ARGBEGIN{
	case 'p': prompt = EARGF(usage); break;
	default:
		usage();
	}ARGEND

	print("%s", prompt);
	for(;;){
		n = read(0, &in, 1);
		if(n != 1)
			break;

		switch(in){
		case '\n':
		case ';':
			buf[offset] = '\0';
			offset = 0;
			argcount = gettokens(buf, args, MAXARG, " ");
			c = commandparse(argcount, args);
			commandexec(c);
			//destroycommand(c);
			print("%s", prompt);
			break;
		default:
			buf[offset++] = in;
		}
	}
	
}