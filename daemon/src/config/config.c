// Custom Headers
#include <config.h>
#include <indexmap.h>

// Standard Headers
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

t_config	config_init(void)
{
	t_config	config;

	config = (t_config){
		.numprocs = 1,
		.umask = 022,
		.autostart = AUTOSTART_TRUE,
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

t_config	*new_config(void)
{
	t_config	*config;

	config = malloc(sizeof(*config));
	if (!config)
		return (NULL);
	*config = config_init();
	return (config);
}

void	config_destroy(void *element)
{
	t_config	*config;

	if (!element) return;

	config = element;
	free(config->name);
	free(config->cmd);
	free(config->workingdir);
	free(config->stdout_path);
	free(config->stderr_path);
	if (config->env.destroy) config->env.destroy(&config->env);
	*config = (t_config){0};
}

void	delete_config(t_config *config)
{
	if (!config)
		return ;
	config_destroy(config);
	free(config);
}

void	config_move(t_config *dest, t_config *src)
{
	if (!dest || !src || dest == src)
		return ;
	config_destroy(dest);
	*dest = *src;
	*src = (t_config){0};
}
int	config_compare_name(const void *first, const void *second, size_t size)
{
	(void)size;
	return (strcmp(((t_config*)first)->name, ((t_config*)second)->name));
}

static int	signal_number(const char *name)
{
	static const struct {
		const char	*name;
		int			number;
	} signals[] = {
		{"HUP", SIGHUP}, {"INT", SIGINT}, {"QUIT", SIGQUIT},
		{"KILL", SIGKILL}, {"USR1", SIGUSR1}, {"USR2", SIGUSR2},
		{"TERM", SIGTERM}, {"CONT", SIGCONT}, {"STOP", SIGSTOP},
		{"TSTP", SIGTSTP}
	};
	const size_t	signal_count = sizeof(signals) / sizeof(signals[0]);
	size_t		i;

	for (i = 0; i < signal_count; i++)
	{
		if (strcmp(name, signals[i].name) == 0)
			return (signals[i].number);
	}
	return (-1);
}

void	copy_string_from_scalar(const char *source, char **destination)
{
	char	*copy;

	if (source == NULL || destination == NULL)
		return ;
	copy = strdup(source);
	if (copy != NULL)
		*destination = copy;
}

void	copy_unsigned_from_scalar(const char *source,
								  unsigned int *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (unsigned int)strtoul(source, NULL, 10);
}

void	copy_mode_from_scalar(const char *source, mode_t *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (mode_t)strtoul(source, NULL, 8);
}

void	copy_autostart_from_scalar(const char *source,
								  t_autostart *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = AUTOSTART_INVALID;
	if (strcmp(source, "false") == 0)
		*destination = AUTOSTART_FALSE;
	else if (strcmp(source, "true") == 0)
		*destination = AUTOSTART_TRUE;
}

void	copy_autorestart_from_scalar(const char *source,
									 t_autorestart *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = AUTORESTART_INVALID;
	if (strcmp(source, "never") == 0 || strcmp(source, "false") == 0)
		*destination = AUTORESTART_NEVER;
	else if (strcmp(source, "always") == 0 || strcmp(source, "true") == 0)
		*destination = AUTORESTART_ALWAYS;
	else if (strcmp(source, "unexpected") == 0)
		*destination = AUTORESTART_UNEXPECTED;
}

void	copy_signal_from_scalar(const char *source, int *destination)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = signal_number(source);
}

bool	config_validate(t_config *cfg)
{
	static const int	valid_signals[] = {
		SIGHUP, SIGINT, SIGQUIT, SIGKILL, SIGUSR1,
		SIGUSR2, SIGTERM, SIGCONT, SIGSTOP, SIGTSTP
	};
	const size_t		signal_count = sizeof(valid_signals)
		/ sizeof(valid_signals[0]);

	if (cfg == NULL || cfg->name == NULL || cfg->name[0] == '\0'
		|| cfg->cmd == NULL || cfg->cmd[0] == '\0' || cfg->numprocs == 0
		|| (cfg->umask & ~(mode_t)0777) != 0)
		return (false);

	if ((cfg->autostart != AUTOSTART_FALSE
			&& cfg->autostart != AUTOSTART_TRUE)
		|| (cfg->autorestart != AUTORESTART_NEVER
		&& cfg->autorestart != AUTORESTART_ALWAYS
		&& cfg->autorestart != AUTORESTART_UNEXPECTED))
		return (false);

	for (size_t index = 0; index < signal_count; index++)
	{
		if (cfg->stopsignal == valid_signals[index])
			return (true);
	}
	return (false);
}
