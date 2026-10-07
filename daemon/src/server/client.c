#include <server.h>
#include <poll.h>

void	client_destroy(void *const client)
{
	t_pollfd *const	pollfd = (t_pollfd*)client;

	if (pollfd->fd > -1)
		close(pollfd->fd);
}

void	client_update(t_pollfd *const pollfd)
{
	t_response	res;
	void		*payload;

	if (!(pollfd->revents & POLLIN))
		return ;
	if (pollfd->revents & (POLLHUP | POLLERR) ||
		!ipc_recv(pollfd->fd, &res, &payload))
	{
		close(pollfd->fd);
		pollfd->fd = -1;
		return ;
	}
}

void	client_clean_array(t_uvector *const clients)
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