#include <config.h>
#include <stdbool.h>

void	config_destroy(void *element)
{
	t_config	*config;

	config = element;
	free(config->name);
	free(config->cmd);
	free(config->workingdir);
	free(config->stdout_path);
	free(config->stderr_path);
	config->env.destroy(&config->env);
	*config = (t_config){0};
}

static t_config	config_default(void)
{
	t_config	config;

	config = (t_config){
		.numprocs = 1,
		.umask = 022,
		.autostart = true,
		.autorestart = AUTORESTART_UNEXPECTED,
		.startretries = 3,
		.starttime = 1,
		.stopsignal = SIGTERM,
		.stoptime = 10,
		.env = init_strmap(sizeof(char *))
	};
	config.exitcodes[0] = true;
	vector_strigfy(&config.env.values);
	return (config);
}

static void	copy_string_from_scalar(const char *source, char **destination)
{
	char	*copy;

	if (source == NULL || destination == NULL)
		return ;
	copy = strdup(source);
	if (copy != NULL)
		*destination = copy;
}

static void	copy_unsigned_from_scalar(const char *source,
		unsigned int *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (unsigned int)strtoul(source, NULL, 10);
}

static void	copy_mode_from_scalar(const char *source, mode_t *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (mode_t)strtoul(source, NULL, 8);
}

static void	copy_bool_from_scalar(const char *source, bool *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (strcmp(source, "true") == 0);
}

static void	copy_autorestart_from_scalar(const char *source,
		t_autorestart *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = AUTORESTART_UNEXPECTED;
	if (strcmp(source, "never") == 0 || strcmp(source, "false") == 0)
		*destination = AUTORESTART_NEVER;
	else if (strcmp(source, "always") == 0 || strcmp(source, "true") == 0)
		*destination = AUTORESTART_ALWAYS;
}

static void	copy_signal_from_scalar(const char *source, int *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = signal_number(source);
}
