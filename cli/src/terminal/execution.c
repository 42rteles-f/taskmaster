#include <terminal.h>
#include "terminal_internal.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static void	print_help(void)
{
	printf(
		"Commands:\n"
		"  status             show all configured programs\n"
		"  start <program>    start a program\n"
		"  stop <program>     stop a program\n"
		"  restart <program>  restart a program\n"
		"  reload             reload the configuration\n"
		"  shutdown           stop Taskmaster\n"
		"  exit | quit        close this client\n"
	);
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

void	command_execute_request(t_command *this)
{
	t_ipc_message	response;
	t_control		*control;

	control = this->control;
	response = ipc_message_init();
	errno = 0;
	if (ipc_send(control->socket, &this->message) < 0
		|| ipc_recv(control->socket, &response) < 0)
	{
		if (errno)
			fprintf(stderr, "taskmasterctl: communication failed: %s\n",
				strerror(errno));
		else
			fprintf(stderr,
				"taskmasterctl: communication failed: connection closed\n");
		ipc_message_clear(&response);
		control->status = 1;
		control->running = false;
		return ;
	}
	print_response(&response);
	ipc_message_clear(&response);
	control->status = (response.header.type == IPC_INVALID);
	if (this->message.header.type == IPC_SHUTDOWN)
		control->running = false;
}

void	command_execute_help(t_command *this)
{
	print_help();
	this->control->status = 0;
}

void	command_execute_exit(t_command *this)
{
	this->control->running = false;
}

void	run_input(t_control *this)
{
	this->command.execute(&this->command);
}
