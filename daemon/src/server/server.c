#include <supervisor.h>
#include <server.h>
#include <client.h>

#include <sys/socket.h>
#include <stddef.h>
#include <errno.h>
#include <poll.h>

void	server_incoming_connections(void)
{
	t_server *const	this = server();
	int				new_fd;

	if (!(((t_client*)this->clients->data)[0].pollfd.revents & POLLIN))
		return ;

	new_fd = ipc_accept(this->socket);
	if (new_fd < 0)
	{
		ERROR_SEND;
		return ;
	}
	*(t_client*)this->clients->emplace(this->clients) = client_init(new_fd);
}

void	server_poll_update(void *const client_arg, size_t index)
{
	t_client *const	client = client_arg;
	t_ipc_message	response;

	if (index < 1)
		server_incoming_connections();
	else if (client_update(client))
	{
		response = supervisor()->handle_request(&client->message);
		client_send(client, &response);
		ipc_message_destroy(&response);
	}

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

	return (true);
}
