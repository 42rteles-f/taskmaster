#include <terminal.h>

#include <errno.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void	control_d(t_control *get)
{
	if (get->interactive)
		write(STDOUT_FILENO, "exit\n", 5);
	get->running = false;
}

static volatile sig_atomic_t	g_interrupt;

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

static bool	setup_signals(bool interactive)
{
	struct sigaction	action;

	memset(&action, 0, sizeof(action));
	sigemptyset(&action.sa_mask);
	action.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &action, NULL) < 0)
		return (false);
	if (!interactive)
		return (true);
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

bool	setup(t_control *this, bool interactive)
{
	*this = (t_control){
		.socket = -1,
		.status = EXIT_SUCCESS,
		.interactive = interactive,
		.running = true
	};
	if (!setup_signals(interactive))
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
	free(this->input);
	this->input = NULL;
	this->payload = NULL;
	this->command = IPC_EMPTY;
	this->action = TERMINAL_REQUEST;
}

void	end_terminal(t_control *this)
{
	input_reset(this);
	if (this->socket >= 0)
		close(this->socket);
	this->socket = -1;
	rl_event_hook = NULL;
	rl_clear_history();
}
