#include "./headers/supervisor.h"
#include "./headers/server.h"
// #include <taskmaster.h>

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
	supervisor()->sync(config);
	server()->start();
}
