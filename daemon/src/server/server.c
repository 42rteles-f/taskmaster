// Custom Headers
#include <supervisor.h>
#include <server.h>
#include "pollable.h"
#include "server_internal.h"
// Standard Headers
#include <sys/socket.h>
#include <stddef.h>
#include <errno.h>
#include <poll.h>

t_server	*server(void)
{
	static t_server	this = {
		.socket = -1,
		.init = server_init,
		.start = server_start,
		.destroy = server_destroy
	};

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
		if (((t_pollfd*)keys->data)[index].fd < 0) {
			printf("removed client: %i\n", ((t_pollfd*)keys->data)[index].fd);
			clients->remove_at(clients, index);
		}
		else
			index++;
	}
}

void	call_pollable_handler(void *key, void *value, size_t index)
{
	t_pollable *const	pollable = value;

	(void)index;
	pollable->pollfd = key;
	printf("here call pollable %i\n", pollable->pollfd->fd);
	pollable->handler(pollable);
}

static inline bool	server_has_connection_request(t_server *this)
{
	t_pollfd	*const socket = this->pollables->keys.find(
		&this->pollables->keys, &(t_pollfd){.fd = this->socket});

	return (socket != NULL && (socket->revents & POLLIN));
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
	t_pollfd		new_pollfd;
	int				new_fd;

	TRACE;
	HERE;
	if (!server_has_connection_request(this))
		return ;
	THERE;
	new_fd = ipc_accept(this->socket);
	if (new_fd < 0)
	{
		ERROR_SEND;
		return ;
	}
	new_pollfd = pollfd_init(new_fd);
	new_client = pollable_init();
	new_client.handler = server_handle_pollable;
	printf("new client added %i\n", new_pollfd.fd);
	this->pollables->set(this->pollables, &new_pollfd, &new_client);
}

void	server_handle_pollable(void *const arg)
{
	t_pollable *const	pollable = arg;
	t_ipc_message		response;

	if (!pollable_read(pollable)) return ;

	printf("pollable message\n");
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

		this->pollables->for_each(this->pollables, call_pollable_handler);
		server_clean_pollables(this->pollables);
		printf("loop end\n");
	}

	return (true);
}
