#include <supervisor.h>
#include <server.h>
#include <parser.h>

int	main(int argc, char **argv)
{
	t_uovector	*configs;
	const char	*config_path;

	config_path = (argc > 1) ? argv[1] : SUPERVISOR_CONFIG_FILE;
	configs = parse_yaml_file(config_path);
	if (configs == NULL)
		return (1);

	supervisor()->init();
	// supervisor()->sync(configs);
	// server()->start();
	supervisor()->destroy();
}
