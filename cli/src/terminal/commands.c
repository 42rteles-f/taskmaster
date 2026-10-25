#include <terminal.h>
#include "terminal_internal.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct s_command_definition
{
	const char			*name;
	t_command_builder	build;
}	t_command_definition;

static bool	status_command(t_control *this);
static bool	start_command(t_control *this);
static bool	stop_command(t_control *this);
static bool	restart_command(t_control *this);
static bool	reload_command(t_control *this);
static bool	shutdown_command(t_control *this);
static bool	help_command(t_control *this);
static bool	exit_command(t_control *this);
static bool	invalid_command(t_control *this);

static const t_command_definition	g_commands[] = {
	{"status", status_command},
	{"start", start_command},
	{"stop", stop_command},
	{"restart", restart_command},
	{"reload", reload_command},
	{"shutdown", shutdown_command},
	{"help", help_command},
	{"exit", exit_command},
	{"quit", exit_command},
	{NULL, invalid_command}
};

bool	normalize_input(t_control *this)
{
	this->split = ft_split_space(this->input);

	return (this->split && this->split[0]);
}

static bool	validate_no_arguments(t_control *this)
{
	if (!this->split[1])
		return (true);
	fprintf(stderr, "taskmasterctl: usage: %s\n", this->split[0]);
	this->status = 2;
	return (false);
}

static bool	validate_program(t_control *this)
{
	if (this->split[1] && !this->split[2])
		return (true);
	fprintf(stderr, "taskmasterctl: usage: %s <program>\n", this->split[0]);
	this->status = 2;
	return (false);
}

static bool	setup_request(t_control *this, t_ipc_type type, char *payload)
{
	size_t	payload_len;

	payload_len = (payload) ? strlen(payload) + 1 : 0;
	if (payload_len > IPC_MAX_PAYLOAD || payload_len > UINT32_MAX)
	{
		fprintf(stderr, "taskmasterctl: command payload is too large\n");
		this->status = 2;
		return (false);
	}
	if (!ipc_message_set(&this->command.message, type, payload, payload_len))
	{
		fprintf(stderr, "taskmasterctl: cannot allocate request payload\n");
		this->status = 1;
		return (false);
	}
	this->command.execute = command_execute_request;
	return (true);
}

static bool	status_command(t_control *this)
{
	if (!validate_no_arguments(this))
		return (false);
	return (setup_request(this, IPC_STATUS, NULL));
}

static bool	start_command(t_control *this)
{
	if (!validate_program(this))
		return (false);
	return (setup_request(this, IPC_START, this->split[1]));
}

static bool	stop_command(t_control *this)
{
	if (!validate_program(this)) {
		printf("not valid\n");
		return (false);
	}
	return (setup_request(this, IPC_STOP, this->split[1]));
}

static bool	restart_command(t_control *this)
{
	if (!validate_program(this))
		return (false);
	return (setup_request(this, IPC_RESTART, this->split[1]));
}

static bool	reload_command(t_control *this)
{
	if (!validate_no_arguments(this))
		return (false);
	return (setup_request(this, IPC_RELOAD, NULL));
}

static bool	shutdown_command(t_control *this)
{
	if (!validate_no_arguments(this))
		return (false);
	return (setup_request(this, IPC_SHUTDOWN, NULL));
}

static bool	help_command(t_control *this)
{
	if (!validate_no_arguments(this))
		return (false);
	this->command.execute = command_execute_help;
	return (true);
}

static bool	exit_command(t_control *this)
{
	if (!validate_no_arguments(this))
		return (false);
	this->command.execute = command_execute_exit;
	return (true);
}

static bool	invalid_command(t_control *this)
{
	fprintf(stderr, "taskmasterctl: unknown command: %s\n", this->split[0]);
	fprintf(stderr, "Type 'help' for available commands.\n");
	this->status = 2;
	return (false);
}

bool	structure_commands(t_control *this)
{
	size_t	index;

	index = 0;
	while (
		g_commands[index].name
		&& strcmp(this->split[0], g_commands[index].name) != 0
	) index++;

	return (g_commands[index].build(this));
}
