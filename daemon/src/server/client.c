#include <client.h>
#include <server.h>
#include "pollfd.c"

#include <poll.h>

void	pollable_destroy(void *const client)
{
	t_pollable *const	this = client;

	delete_pollfd(this->pollfd);
	this->pollfd = NULL;
	ipc_message_reset(&this->message);
	this->handler = pollable_do_nothing;
}

void	pollable_clear_data(t_pollable *const this)
{
	ipc_message_reset(&this->message);
}

bool	pollable_read_message(t_pollable *const this)
{
	if (!(this->pollfd->revents & POLLIN))
		return (false);
	if (this->pollfd->revents & (POLLHUP | POLLERR) ||
		0 != ipc_recv(this->pollfd->fd, &this->message.header, &this->message.payload))
	{
		close(this->pollfd->fd);
		this->pollfd->fd = -1;
		return (false);
	}
	return (true);
}

int	pollable_send_message(t_pollable *const this, t_ipc_message *const response)
{
	if (response->header.type == IPC_EMPTY)
		return (0);
	if (0 != ipc_send(this->pollfd->fd, &response->header, response->payload))
	{
		close(this->pollfd->fd);
		this->pollfd->fd = -1;
		return (-1);
	}

	return (0);
}

//unorderd vector means last member is copied into removed index
//upon removal, we need to check the same index again.
void	pollable_clean_array(t_uovector *const clients)
{
	size_t	index;

	index = 0;
	while (index < clients->size)
	{
		if (((t_pollfd*)clients->at(clients, index))->fd < 0)
			clients->remove_at(clients, index);
		else
			index++;
	}
}

void	pollable_do_nothing(struct event_source *const source) { return ; };

void	pollable_init(int fd)
{
	return ((t_pollable) {
		.pollfd = new_pollfd(fd),
		.message = ipc_message_init(),
		.handler = pollable_do_nothing,
	});
}