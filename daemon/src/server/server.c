#include <server.h>
#include <sys/socket.h>
#include <stddef.h>
#include <errno.h>
#include <poll.h>

void	server_incoming_connections(void)
{
	t_server *const	this = server();
	int				new_fd;

	if (!(this->socket & POLLIN))
		return ;

	new_fd = accept(this->socket, NULL, NULL);
	if (new_fd < 0)
	{
		ERROR_SEND;
		return ;
	}
	*(t_pollfd*)this->clients->emplace(this->clients) = (t_pollfd) {
		.fd = new_fd,
		.events = POLLIN,
		.revents = 0
	};
}

void	server_poll_update(void *const client, size_t index)
{
	t_pollfd *const	pollfd = (t_pollfd*)client;

	if (index < 1)
		server_incoming_connections();
	else if (pollfd->revents & (POLLHUP | POLLERR))
	{
		close(pollfd->fd);
		pollfd->fd = -1;
		return ;
	}
	ipc_recv(pollfd->fd, NULL, 0, MSG_PEEK | MSG_DONTWAIT);
}

void	clean_clients(t_uvector *const clients)
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

bool server_start(void)
{
	t_server *const this = server();

	if (!server_open_connections(this))
		return (false);

	while (this->online)
	{
		if (poll(this->clients->data, this->clients->size, -1) < 0)
		{
			if (errno == EINTR) continue;

			ERROR_SEND;
			return (false);
		}

		this->clients->for_each(this->clients, server_poll_update);
		clean_clients(this->clients);
	}
}
