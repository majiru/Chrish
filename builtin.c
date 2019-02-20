#include <u.h>
#include <libc.h>

#include "chrish.h"

#define argvcmp(args,s) strcmp(args[0], s) == 0

void
execcd(int argc, char *argv[])
{
	char *home, *env;

	if(argc > 1){
		if(chdir(argv[1]) < 0)
			perror(argv0);
		return;
	}

	#ifdef _PLAN9_SOURCE
		home = "home";
	#endif

	#ifdef _POSIX_SOURCE
		home = "HOME";
	#endif

	env = getenv(home);

	if(chdir(env) < 0)
			perror(argv0);
	free(env);
}

int
execbuiltin(int argc, char *argv[])
{
	char buf[MAXPATH];
	char *env;

	if(argvcmp(argv, "exit"))
		exits(nil);

	if(argvcmp(argv, "pid")){
		print("%d\n", getpid());
		return 1;
	}

	if(argvcmp(argv, "ppid")){
		print("%d\n", getppid());
		return 1;
	}

	if(argvcmp(argv, "pwd")){
		print("%s\n", getwd(buf, MAXPATH));
		return 1;
	}

	if(argvcmp(argv, "set")){
		if(argc == 3)
			putenv(argv[1], argv[2]);
		else
			fprint(2, "Usage: set [var] [value]\n");

		return 1;
	}

	if(argvcmp(argv, "get")){
		if(argc == 2){
			env = getenv(argv[1]);
			print("%s\n", env);
			free(env);
		}else
			fprint(2, "Usage: get [var]\n");

		return 1;
	}

	if(argvcmp(argv, "cd")){
		execcd(argc, argv);
		return 1;
	}
		
	return 0;
}