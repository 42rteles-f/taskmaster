#include <supervisor.h>
#include <server.h>
#include <parser.h>

int	main(int argc, char **argv)
{
	t_uovector	*configs;
	const char	*config_path;

	config_path = (argc > 1) ? argv[1] : SUPERVISOR_CONFIG_FILE;
	configs = parse_yaml_file(config_path);

	supervisor()->init();
	supervisor()->sync(configs);
	server()->init();
	server()->start();
	server()->destroy();
	supervisor()->destroy();
}
