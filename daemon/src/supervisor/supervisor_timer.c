// Custom Headers
#include <supervisor.h>
#include "supervisor_internal.h"
#include <program.h>

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

void	call_program_update(void* element, void *time, size_t index)
{
	(void)index;
	program_update(element, time);
}

void	supervisor_periodic_check(t_supervisor *this)
{
	struct timespec		now;

	clock_gettime(CLOCK_MONOTONIC, &now);
	this->programs->for_each_ctx(this->programs, call_program_update, &now);
}

void	supervisor_handle_timer(struct pollfd *pollfd, void *context)
{
	t_supervisor *const	this = context;

	if (!(pollfd->revents & POLLIN))
		return ;
	if (read(pollfd->fd, &(uint64_t){}, sizeof(uint64_t)) < 0)
		return ;

	supervisor_periodic_check(this);
}
