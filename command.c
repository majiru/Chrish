#include <u.h>
#include <libc.h>

#include "chrish.h"

Command*
createcommand(void)
{
	Pipe p = {FD, 0};
	Command *c = mallocz(sizeof(Command), 1);
	c->argc = 0;
	c->args = malloc(sizeof(char*) * MAXWORD);
	if(c->args == nil)
		sysfatal("%s: Failed to allocate args", argv0);

	c->in = p;

	p.fd = 1;
	c->out = p;

	p.fd = 2;
	c->err = p;

	return c;
}

void
destroypipe(Pipe *p)
{
	if(p->type == COMMAND && p->c != nil)
		destroycommand(p->c);
}

void
destroycommand(Command *c)
{
	int i;

	destroypipe(&c->in);
	destroypipe(&c->out);
	destroypipe(&c->err);

	for(i = 0; i < c->argc; i++)
		free(c->args[i]);

	free(c);
}

int
createoropen(char *file, int mode, int perm)
{
	int fd;
	if((fd = open(file, mode)) < 0)
		return create(file, mode, perm);

	return fd;
}

/* Treats '>file' and '> file' the same */
char*
cleanredir(int n, char *args[], int *i)
{
	if(strlen(args[*i]) == 1 && *i+1 < n)
		return args[++(*i)];
	else
		return args[*i]+1;
}

/* | pipe
 * < stdin
 * > stdout
*/
Command*
commandparse(int n, char *args[])
{
	int i;
	Command *c = createcommand();

	for(i = 0; i < n; i++){
		switch(args[i][0]){
		case '>':
			c->out.fd = createoropen(cleanredir(n, args, &i), OWRITE | OTRUNC, 0777);
			if(c->out.fd < 0)
				goto error;
			break;
		case '<':
			c->in.fd = createoropen(cleanredir(n, args, &i), OREAD, 0777);
			if(c->in.fd < 0)
				goto error;
			break;
		case '|':
			c->out.type = COMMAND;
			c->out.c = commandparse(n - (i + 1), args + i + 1);
			if(c->out.c == nil)
				goto error;
			return c;
			break;
		default:
			c->args[c->argc++] = strdup(args[i]);
		}
	}
	return c;

error:
	destroycommand(c);
	perror(argv0);
	return nil;
}

void
waitprint(void)
{
	Waitmsg *w;

	w = wait();
	if(w == nil)
		return;
	
	if(chatty)
		print("Pid: %d\tUser code: %uld Syscalls: %uld Total: %uldt\t %s\n",
			w->pid, w->time[0], w->time[1], w->time[2], w->msg);

	free(w);
}

void
commandpipe(Command *c)
{
	int inpipe[2];
	switch(c->in.type){
	case COMMAND:
		pipe(inpipe);
		if(fork() == 0){
			c->in.c->out.fd = inpipe[1];
			commandexec(c->in.c);
		}
		c->in.type = FD;
		c->in.fd = inpipe[0];	
		/* fallthrough */
	case FD:
		dup(c->in.fd, 0);
		break;
	}

	int outpipe[2];
	switch(c->out.type){
	case COMMAND:
		pipe(outpipe);
		if(fork() == 0){
			c->out.c->in.fd = outpipe[1];
			commandexec(c->out.c);
		}
		c->out.type = FD;
		c->out.fd = outpipe[0];
		/* fallthrough */
	case FD:
		dup(c->out.fd, 1);
		break;
	}

	int errpipe[2];
	switch(c->err.type){
	case COMMAND:
		pipe(errpipe);
		if(fork() == 0){
			c->err.c->in.fd = errpipe[1];
			commandexec(c->err.c);
		}
		c->err.type = FD;
		c->err.fd = errpipe[0];		
		/* fallthrough */
	case FD:
		dup(c->err.fd, 2);
		break;
	}

}

void
commandexec(Command *c)
{
	int pid;
	char block;
	
	if(c->argc == 0)
		return;
	
	if(execbuiltin(c->argc, c->args) == 1)
		return;

	if(c->args[c->argc - 1][0] == '&'){
		block = 0;
		c->argc--;
	}else
		block = 1;

	pid = fork();

	if(pid != 0){
		if(chatty)
			print("%s: %d\n", c->args[0], pid);
		if(block)
			waitprint();
		return;
	}

	if(!block)
		if(fork() != 0){
			waitprint();
			exits(nil);
		}

	c->args[c->argc] = nil;

	commandpipe(c);

	#ifdef _PLAN9_SOURCE
		char buf[MAXPATH];
		snprint(buf, MAXPATH, "/bin/%s", c->args[0]);
		exec(buf, c->args);
	#endif

	#ifdef _POSIX_SOURCE
		execvp(c->args[0], c->args);
	#endif

	exits(nil);
}
