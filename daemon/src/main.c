// Custom Headers
#include <supervisor.h>
#include <server.h>
#include <parser.h>
#include <vector.h>

int	main(int argc, char **argv)
{
	t_uovector	*configs;
	const char	*config_path;

	config_path = (argc > 1) ? argv[1] : SUPERVISOR_CONFIG_FILE;
	configs = parse_yaml_file(config_path);

	if (
		supervisor()->init() &&
		supervisor()->sync(&configs) &&
		server()->init(supervisor_handle_request) &&
		server()->register_event(supervisor()->signal_pipe[0], supervisor_handle_pipe, supervisor()) &&
		server()->register_event(supervisor()->timer_fd, supervisor_handle_timer, supervisor())
	) {
		server()->start();
		server()->unregister_event(supervisor()->signal_pipe[0]);
		server()->unregister_event(supervisor()->timer_fd);
	}

	supervisor()->destroy();
	server()->destroy();
	delete_vector(configs);
	return (0);
}
