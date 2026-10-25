// Custom Headers
#include <server.h>
#include "pollable.h"
#include "server_internal.h"
// Standard Headers
#include <poll.h>

void	pollable_destroy(void *const arg)
{
	t_pollable *const	this = arg;

	this->pollfd = NULL;
	ipc_message_clear(&this->message);
	this->handler = pollable_do_nothing;
}

void	pollable_clear_data(t_pollable *const this)
{
	ipc_message_clear(&this->message);
}

bool	pollable_read(t_pollable *const this)
{
	if (this->pollfd == NULL || !(this->pollfd->revents & POLLIN))
		return (false);

	ipc_message_clear(&this->message);
	if (this->pollfd->revents & (POLLHUP | POLLERR) ||
		0 != ipc_recv(this->pollfd->fd, &this->message))
	{
		close(this->pollfd->fd);
		this->pollfd->fd = -1;
		return (false);
	}
	return (true);
}

int	pollable_send(t_pollable *const this, t_ipc_message *const response)
{
	if (response->header.type == IPC_EMPTY)
		return (0);
	if (this->pollfd == NULL
		|| 0 != ipc_send(this->pollfd->fd, response))
	{
		if (this->pollfd != NULL)
		{
			close(this->pollfd->fd);
			this->pollfd->fd = -1;
		}
		return (-1);
	}

	return (0);
}

void	pollable_do_nothing(void *source)
{
	(void)source;
}

t_pollable	pollable_init(void)
{
	return ((t_pollable) {
		.message = ipc_message_init(),
		.handler = pollable_do_nothing,
	});
}
