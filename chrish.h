#define MAXWORD 512
#define MAXPATH 512
#define MAXARG 256

enum {
	COMMAND,
	FILE,
};

typedef struct Command Command;
typedef struct Pipe Pipe;

struct Pipe{
	char type;
	union {
		Command *c;
		int fd;	
	};
};

struct Command {
	int argc;
	char **args;
	Pipe in, out, err;
};

/* command.c */
void		destroypipe(Pipe *p);
void		destroycommand(Command *c);
Command*	commandparse(int n, char *args[]);
void		commandexec(Command *c);

/* builtin.c */
void	execcd(int argc, char *argv[]);
int		execbuiltin(int argc, char *argv[]);

