// Custom Headers
#include <supervisor.h>
#include <server.h>
#include "pollable.h"
// Standard Headers
#include <sys/socket.h>
#include <stddef.h>
#include <errno.h>
#include <poll.h>

t_server	*server(void)
{
	static t_server	this;

	return (&this);
}

//unorderd vector means last member is copied into removed index
//upon removal, we need to check the same index again.
void	server_clean_pollables(t_indexmap *const clients)
{
	size_t				index;
	t_uovector	*const	keys = &clients->keys;

	index = 0;
	while (index < keys->size)
	{
		if (((t_pollfd*)keys->data)[index].fd < 0)
			clients->remove_at(clients, index);
		else
			index++;
	}
}

void	call_pollable_handler(void *arg, size_t index)
{
	(void)index;
	((t_pollable*)arg)->handler(arg);
}

static inline bool	server_has_connection_request(t_server *this)
{
	t_pollable	*const server = this->pollables->get(
		this->pollables,
		&(t_pollfd){ .fd = this->socket }
	);
	return (server->pollfd->revents & POLLIN);
}

void	server_handle_pipe_trigger(void *const arg)
{
	t_pollable *const	pollable = arg;

	// if (!pollable_read(pollable))

}

void	server_handle_connections(void *const arg)
{
	t_server *const	this = ((t_pollable*)arg)->context;
	t_pollable		new_client;
	int				new_fd;

	if (!server_has_connection_request(this))
		return ;

	new_fd = ipc_accept(this->socket);
	if (new_fd < 0)
	{
		ERROR_SEND;
		return ;
	}
	new_client = pollable_init(new_fd);
	new_client.handler = server_handle_pollable;
	this->pollables->set(this->pollables, new_client.pollfd, &new_client);
}

void	server_handle_pollable(void *const arg)
{
	t_pollable *const	pollable = arg;
	t_ipc_message		response;

	if (!pollable_read(pollable)) return ;

	response = supervisor()->handle_request(&pollable->message);
	pollable_send(pollable, &response);
	ipc_message_reset(&pollable->message);
	ipc_message_reset(&response);
}

bool server_start(void)
{
	t_server *const this = server();

	if (!server_open_connections(this))
		return (false);

	while (this->online)
	{
		if (poll(this->pollables->keys.data, this->pollables->keys.size, -1) < 0)
		{
			if (errno == EINTR) continue;

			ERROR_SEND;
			return (false);
		}

		this->pollables->values.for_each(this->pollables, call_pollable_handler);
		server_clean_pollables(this->pollables);
	}

	return (true);
}
