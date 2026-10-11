#include "./headers/supervisor.h"
#include "./headers/server.h"
#include "./headers/parser.h"
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
	t_uovector	*configs;
	const char	*config_path;

	config_path = SUPERVISOR_CONFIG_FILE;
	if (argc > 1)
		config_path = argv[1];
	configs = parse_yaml_file(config_path);
	if (configs == NULL)
		return (1);
	supervisor()->init();
	supervisor()->sync(configs);
	server()->start();
}
