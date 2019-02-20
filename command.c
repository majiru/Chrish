#include <u.h>
#include <libc.h>

#include "chrish.h"

Command*
createcommand(void)
{
	Pipe p = {FILE, 0};
	Command *c = mallocz(sizeof(Command), 1);
	c->argc = 0;
	c->args = malloc(sizeof(char*) * MAXWORD);

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

/* Treats '>file' and '> file' the same */
int
cleanandopen(int n, char *args[], int i, int mode)
{
	if(strlen(args[i]) == 1 && i+1 < n)
		return open(args[i+1], mode);
	else
		return open(args[i]+1, mode);
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
			c->out.fd = cleanandopen(n, args, i, OWRITE | OTRUNC);
			if(c->out.fd < 0)
				goto error;
			break;
		case '<':
			c->in.fd = cleanandopen(n, args, i, OREAD);
			if(c->in.fd < 0)
				goto error;
			break;
		case '|':
			c->out.c = commandparse(n - i, args+i);
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

	print("Pid: %d\tUser code: %uld Syscalls: %uld Total: %uldt\t %s\n",
		w->pid, w->time[0], w->time[1], w->time[2], w->msg);

	free(w);
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

	if(c->args[c->argc - 1][0] == '&')
		block = 0;
	else
		block = 1;

	pid = fork();

	if(pid != 0){
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