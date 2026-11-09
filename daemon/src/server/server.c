// Custom Headers
#include <server.h>
#include <indexmap.h>
#include <ipc.h>
#include "server_internal.h"

// Standard Headers
#include <stdbool.h>
#include <errno.h>
#include <poll.h>
#include <stddef.h>
#include <stdio.h>

t_server	*server(void)
{
	static t_server	this = {
		.socket = -1,
		.init = server_init,
		.start = server_start,
		.stop = server_stop,
		.destroy = server_destroy,
		.register_event = server_register_event,
		.unregister_event = server_unregister_event,
	};

	return (&this);
}

//unorderd vector means last member is copied into removed index
//upon removal, we need to check the same index again.
void	server_clean_subscriptions(t_indexmap *const subscriptions)
{
	size_t				index;
	t_uovector	*const	keys = &subscriptions->keys;

	index = 0;
	while (index < keys->size)
	{
		if (((t_pollfd*)keys->data)[index].fd < 0) {
			printf("removed client: %i\n", ((t_pollfd*)keys->data)[index].fd);
			subscriptions->remove_at(subscriptions, index);
		}
		else
			index++;
	}
}

void	call_event_handler(void *key, void *value, size_t index)
{
	t_pollfd *const		pollfd = key;
	t_poll_event *const event = value;

	(void)index;
	printf("call event %i\n", pollfd->fd);
	event->handler(pollfd, event->context);
}

static bool	server_ipc_read(t_pollfd *pollfd, t_ipc_message *request)
{
	if (!(pollfd->revents & POLLIN))
		return (false);

	*request = ipc_message_init();
	if (pollfd->revents & (POLLHUP | POLLERR) ||
		ipc_recv(pollfd->fd, request) != 0)
	{
		pollfd_destroy(pollfd);
		return (false);
	}
	return (true);
}

static int	server_ipc_send(t_pollfd *pollfd,
							const t_ipc_message *response)
{
	if (response->header.type == IPC_EMPTY)
		return (0);
	if (ipc_send(pollfd->fd, response) != 0)
	{
		pollfd_destroy(pollfd);
		return (-1);
	}
	return (0);
}

static void	server_handle_ipc_client(t_pollfd *pollfd, void *context)
{
	t_server			*this = context;
	t_ipc_message		request;
	t_ipc_message		response;

	if (!server_ipc_read(pollfd, &request))
		return ;

	printf("ipc message\n");
	response = this->request_handler(&request);
	server_ipc_send(pollfd, &response);
	ipc_message_clear(&request);
	ipc_message_clear(&response);
}

void	server_handle_connections(t_pollfd *pollfd, void *context)
{
	t_server *const	this = context;
	int				new_fd;

	if (!(pollfd->revents & POLLIN))
		return ;
	new_fd = ipc_accept(this->socket);
	if (new_fd < 0)
	{
		ERROR_SEND;
		return ;
	}
	printf("new client added %i\n", new_fd);
	server_register_event(new_fd,
						  server_handle_ipc_client,
						  this);
}

bool server_start(void)
{
	t_server *const this = server();

	if (!server_open_connections(this))
		return (false);

	while (this->online)
	{
		if (poll(this->subscriptions->keys.data,
				 this->subscriptions->keys.size,-1) < 0)
		{
			if (errno == EINTR) continue;

			ERROR_SEND;
			return (false);
		}

		this->subscriptions->for_each(this->subscriptions, call_event_handler);
		server_clean_subscriptions(this->subscriptions);
		printf("loop end\n");
	}

	return (true);
}

void	server_stop(void)
{
	server()->online = false;
}
