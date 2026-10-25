#include <terminal.h>

#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static volatile sig_atomic_t	g_interrupt;


void	control_d(t_control *get)
{
	rl_replace_line("", 1);
	write(STDOUT_FILENO, "exit\n", 5);
	get->running = false;
}

static void	control_c(int signal)
{
	(void)signal;
	g_interrupt = 1;
}

static int	clear_interrupted_line(void)
{
	if (!g_interrupt)
		return (0);
	g_interrupt = 0;
	terminal()->status = 130;
	rl_replace_line("", 0);
	rl_crlf();
	rl_done = 1;
	return (0);
}

static bool	setup_signals(void)
{
	struct sigaction	action = {0};

	action.sa_handler = control_c;
	action.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &action, NULL) < 0)
		return (false);
	action.sa_handler = SIG_IGN;
	action.sa_flags = 0;
	if (sigaction(SIGQUIT, &action, NULL) < 0)
		return (false);
	rl_catch_signals = 0;
	rl_event_hook = clear_interrupted_line;
	return (true);
}

bool	terminal_setup(t_control *this, int argc)
{
	*this = (t_control){
		.socket = ipc_connect(),
		.status = EXIT_SUCCESS,
		.running = true
	};
	if (this->socket < 0)
		return (false);

	if (argc > 1 && !setup_signals())
	{
		fprintf(stderr, "taskmasterctl: cannot configure signals: %s\n",
			strerror(errno));
		this->status = EXIT_FAILURE;
		this->running = false;
		return (false);
	}
	return (true);
}

void	input_reset(t_control *this)
{
	size_t	index;

	if (this->input) free(this->input);
	this->input = NULL;
	ipc_message_reset(&this->message);
	index = 0;
	while (this->split && this->split[index])
		free(this->split[index++]);
	free(this->split);
	this->split = NULL;
	this->action = TERMINAL_REQUEST;
}

void	terminal_destroy(t_control *this)
{
	input_reset(this);
	if (this->socket >= 0) close(this->socket);
	this->socket = -1;
	rl_event_hook = NULL;
	this->running = false;
	rl_clear_history();
}
