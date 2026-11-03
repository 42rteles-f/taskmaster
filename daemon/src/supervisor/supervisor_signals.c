// Custom Headers
#include <supervisor.h>
#include "supervisor_internal.h"

// Standard Headers
#include <stdbool.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>

void	supervisor_handle_signal(const siginfo_t *signal_info)
{
	(void)signal_info;
}

static bool	set_pipe_flags(int fd)
{
	int	flags;

	flags = fcntl(fd, F_GETFL);
	if (flags < 0 || fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0)
		return (false);
	flags = fcntl(fd, F_GETFD);
	if (flags < 0 || fcntl(fd, F_SETFD, flags | FD_CLOEXEC) < 0)
		return (false);
	return (true);
}

bool	supervisor_setup_pipe(void)
{
	t_supervisor	*const this = supervisor();

	if (pipe(this->signal_pipe) < 0)
		return (false);
	if (!set_pipe_flags(this->signal_pipe[0])
		|| !set_pipe_flags(this->signal_pipe[1]))
	{
		close(this->signal_pipe[0]);
		close(this->signal_pipe[1]);
		this->signal_pipe[0] = -1;
		this->signal_pipe[1] = -1;
		return (false);
	}
	return (true);
}

void	supervisor_handle_pipe(struct pollfd *pollfd, void *context)
{
	siginfo_t	signal_info;
	ssize_t		bytes;

	(void)context;
	if (!(pollfd->revents & POLLIN))
		return ;
	while (true)
	{
		bytes = read(pollfd->fd, &signal_info, sizeof(signal_info));
		if (bytes == (ssize_t)sizeof(signal_info))
			supervisor_handle_signal(&signal_info);
		else if (bytes < 0 && errno == EINTR)
			continue ;
		else
			break ;
	}
}

static void	signal_callback(int signal_number, siginfo_t *signal_info,
		void *context)
{
	(void)signal_number;
	(void)context;
	write(supervisor()->signal_pipe[1], signal_info, sizeof(*signal_info));
}

bool	supervisor_set_signals(void)
{
	static const int	signals[] = {
		SIGCHLD, SIGHUP, SIGINT, SIGTERM, SIGQUIT
	};
	struct sigaction	action;
	size_t				total_signals;

	total_signals = sizeof(signals) / sizeof(signals[0]);
	action = (struct sigaction){0};
	action.sa_sigaction = signal_callback;
	action.sa_flags = SA_SIGINFO | SA_RESTART;
	sigemptyset(&action.sa_mask);
	for (size_t index = 0; index < total_signals; index++)
	{
		if (sigaction(signals[index], &action, NULL) < 0)
			return (false);
	}
	return (true);
}
