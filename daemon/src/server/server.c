#include <server.h>
#include <sys/socket.h>
#include <stddef.h>
#include <errno.h>
#include <poll.h>

void	server_incoming_connections(void)
{
	t_server *const	this = server();
	int				new_fd;

	if (!((t_pollfd*)this->clients->data)[0].revents & POLLIN)
		return ;

	new_fd = ipc_accept(this->socket);
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

void	server_poll_update(void *const client_arg, size_t index)
{
	t_pollfd *const	client = (t_pollfd*)client_arg;
	void	*payload;

	if (index < 1)
		server_incoming_connections();
	else
		payload = client_update(client);
	
	if (payload)
		//pass it to supervisor
		return ;
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
