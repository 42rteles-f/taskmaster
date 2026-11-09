// Custom Headers
#include <supervisor.h>
#include <program.h>
#include "supervisor_internal.h"

// Standard Headers
#include <stdbool.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

int	program_has_child(const void *element, const void *pid)
{
	t_program	*const program = element;

	return (program->processes.find(&program->processes, pid) != NULL);
}

void	supervisor_handle_signals(t_supervisor *this)
{
	pid_t			pid;
	t_program		*parent;
	int				status;
	struct timespec	now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	while ((pid = waitpid(-1, &status, WNOHANG)))
	{
        if (pid == -1 && errno == EINTR)
            continue;
		
		parent = this->programs->find_with(this->programs, &pid, program_has_child);
		if (!parent) {
			//TODO log the missing child
			continue;
		}
		program_update_child(parent, pid, status, now);
	}
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
	ssize_t 			bytes;

	if (!(pollfd->revents & POLLIN))
		return ;

	do {
		bytes = read(pollfd->fd, (char[1024]){}, 1024);
	} while (bytes > 0 || (bytes < 0 && errno == EINTR));

	supervisor_handle_signals(context);
}

static void	signal_callback(int signal_number,
							siginfo_t *signal_info,
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
