#include <supervisor.h>
#include <server.h>
#include <client.h>

#include <sys/socket.h>
#include <stddef.h>
#include <errno.h>
#include <poll.h>

void	call_pollable_handler(void *arg, size_t index)
{
	(void)index;
	((t_pollable*)arg)->handler(arg);
}

static inline bool	server_has_connection_request(t_server *this)
{
	return (((t_pollable*)this->pollables->keys.data)->pollfd->revents & POLLIN);
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

	if (!pollable_read_message(pollable)) return ;

	response = supervisor()->handle_request(&pollable->message);
	pollable_send_message(pollable, &response);
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
		clean_clients(this->pollables);
	}

	return (true);
}
