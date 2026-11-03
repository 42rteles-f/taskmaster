// Custom Headers
#include <supervisor.h>
#include "supervisor_internal.h"

// Standard Headers
#include <stdbool.h>
#include <poll.h>
#include <stdint.h>
#include <sys/timerfd.h>
#include <time.h>
#include <unistd.h>

bool	supervisor_activate_timer(void)
{
	t_supervisor *const	this = supervisor();
	struct itimerspec	timer;

	this->timer_fd = timerfd_create(CLOCK_MONOTONIC,
									TFD_NONBLOCK | TFD_CLOEXEC);
	if (this->timer_fd < 0)
		return (false);
	timer = (struct itimerspec){
		.it_value.tv_sec = 1,
		.it_interval.tv_sec = 1,
	};
	if (timerfd_settime(this->timer_fd, 0, &timer, NULL) < 0)
	{
		close(this->timer_fd);
		this->timer_fd = -1;
		return (false);
	}
	return (true);
}

void	supervisor_handle_timer(struct pollfd *pollfd, void *context)
{
	t_supervisor *const	this = context;
	uint64_t			expirations;

	if (!(pollfd->revents & POLLIN))
		return ;
	if (read(pollfd->fd, &expirations, sizeof(expirations)) < 0)
		return ;
	supervisor_check_processes();
}
