#include <terminal.h>

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef struct s_command_definition
{
	const char			*name;
	t_ipc_type			type;
	t_terminal_action	action;
	bool				requires_target;
}	t_command_definition;

static const t_command_definition	g_commands[] = {
	{"status", IPC_STATUS, TERMINAL_REQUEST, false},
	{"start", IPC_START, TERMINAL_REQUEST, true},
	{"stop", IPC_STOP, TERMINAL_REQUEST, true},
	{"restart", IPC_RESTART, TERMINAL_REQUEST, true},
	{"reload", IPC_RELOAD, TERMINAL_REQUEST, false},
	{"shutdown", IPC_SHUTDOWN, TERMINAL_REQUEST, false},
	{"help", IPC_EMPTY, TERMINAL_HELP, false},
	{"exit", IPC_EMPTY, TERMINAL_EXIT, false},
	{"quit", IPC_EMPTY, TERMINAL_EXIT, false},
	{NULL, IPC_EMPTY, TERMINAL_REQUEST, false}
};

static char	*skip_spaces(char *input)
{
	while (*input && isspace((unsigned char)*input))
		input++;
	return (input);
}

bool	normalize_input(t_control *this)
{
	char	*start;
	char	*end;

	if (!this->input)
		return (false);
	start = skip_spaces(this->input);
	if (!*start)
		return (false);
	if (start != this->input)
		memmove(this->input, start, strlen(start) + 1);
	end = this->input + strlen(this->input);
	while (end > this->input && isspace((unsigned char)end[-1]))
		end--;
	*end = '\0';
	return (*this->input != '\0');
}

static const t_command_definition	*find_command(const char *name)
{
	size_t	index;

	index = 0;
	while (g_commands[index].name)
	{
		if (strcmp(name, g_commands[index].name) == 0)
			return (&g_commands[index]);
		index++;
	}
	return (NULL);
}

static bool	invalid_arguments(const t_command_definition *definition,
		const char *payload)
{
	const char	*cursor;

	cursor = payload;
	while (*cursor && !isspace((unsigned char)*cursor))
		cursor++;
	if (definition->requires_target && (!*payload || *cursor))
	{
		fprintf(stderr, "taskmasterctl: usage: %s <program>\n",
			definition->name);
		return (true);
	}
	if (!definition->requires_target && *payload)
	{
		fprintf(stderr, "taskmasterctl: usage: %s\n", definition->name);
		return (true);
	}
	return (false);
}

bool	structure_commands(t_control *this)
{
	const t_command_definition	*definition;
	char						*payload;
	char						*separator;

	separator = this->input;
	while (*separator && !isspace((unsigned char)*separator))
		separator++;
	payload = separator;
	if (*separator)
	{
		*separator = '\0';
		payload = skip_spaces(separator + 1);
	}
	definition = find_command(this->input);
	if (!definition)
	{
		fprintf(stderr, "taskmasterctl: unknown command: %s\n",
			this->input);
		fprintf(stderr, "Type 'help' for available commands.\n");
		this->status = 2;
		return (false);
	}
	if (invalid_arguments(definition, payload))
	{
		this->status = 2;
		return (false);
	}
	this->command = definition->type;
	this->action = definition->action;
	this->payload = payload;
	return (true);
}

static void	print_help(void)
{
	printf("Commands:\n"
		"  status             show all configured programs\n"
		"  start <program>    start a program\n"
		"  stop <program>     stop a program\n"
		"  restart <program>  restart a program\n"
		"  reload             reload the configuration\n"
		"  shutdown           stop Taskmaster\n"
		"  exit | quit        close this client\n");
	fflush(stdout);
}

static void	print_response(const t_ipc_message *response)
{
	size_t	length;
	char	*payload;

	length = response->header.payload_len;
	payload = response->payload;
	if (length > 0 && payload[length - 1] == '\0')
		length--;
	if (length == 0)
		return ;
	fwrite(payload, 1, length, stdout);
	if (payload[length - 1] != '\n')
		putchar('\n');
	fflush(stdout);
}

static bool	execute_request(t_control *this)
{
	t_ipc_message	request;
	t_ipc_message	response;
	size_t			payload_len;

	request = ipc_message_init();
	response = ipc_message_init();
	if (this->socket < 0)
	{
		this->socket = ipc_connect();
		if (this->socket < 0)
		{
			fprintf(stderr, "taskmasterctl: cannot connect to %s: %s\n",
				SOCKET_PATH, strerror(errno));
			this->status = 1;
			this->running = false;
			return (false);
		}
	}
	payload_len = 0;
	if (*this->payload)
		payload_len = strlen(this->payload) + 1;
	if (payload_len > IPC_MAX_PAYLOAD || payload_len > UINT32_MAX)
	{
		fprintf(stderr, "taskmasterctl: command payload is too large\n");
		this->status = 2;
		return (false);
	}
	request.header.type = this->command;
	request.header.payload_len = (uint32_t)payload_len;
	request.payload = this->payload;
	errno = 0;
	if (ipc_send(this->socket, &request.header, request.payload) < 0
		|| ipc_recv(this->socket, &response.header, &response.payload) < 0)
	{
		if (errno)
			fprintf(stderr, "taskmasterctl: communication failed: %s\n",
				strerror(errno));
		else
			fprintf(stderr,
				"taskmasterctl: communication failed: connection closed\n");
		ipc_message_reset(&response);
		this->status = 1;
		this->running = false;
		return (false);
	}
	print_response(&response);
	ipc_message_reset(&response);
	this->status = 0;
	return (true);
}

void	run_input(t_control *this)
{
	if (this->action == TERMINAL_HELP)
	{
		print_help();
		this->status = 0;
	}
	else if (this->action == TERMINAL_EXIT)
		this->running = false;
	else
	{
		execute_request(this);
		if (this->command == IPC_SHUTDOWN)
			this->running = false;
	}
}
