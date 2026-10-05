#include "./headers/supervisor.h"
// #include <taskmaster.h>

typedef struct {
	void	*(*init)();
	void	*(*setup)();
	void	*(*update)();
}	t_supervisor;

t_supervisor	*supervisor()
{
	static	t_supervisor	super = {

	};
	return (&super);
}

/*
If parse fails, daemon continues since it can be updated.
init() should stablish the TCP or unix connection, and listen to cli.
inti() should also stablish signal handler for child process
*/
int	main(int argc, char **argv)
{
	t_config		*config;
	
	config = parse_yaml_file(argv);
	supervisor()->init();
	supervisor()->update(config);
}
