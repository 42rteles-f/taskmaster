// Custom Headers
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
	// supervisor()->sync(configs);
	server()->init(supervisor_handle_request);
	server()->register_reaction(
		supervisor()->signal_pipe[0],
		supervisor(),
		supervisor_handle_pipe
	);
	server()->start();
	server()->unregister_reaction(supervisor()->signal_pipe[0]);
	server()->destroy();
	supervisor()->destroy();
}
