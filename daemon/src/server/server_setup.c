// Custom Headers
#include <server.h>
#include <ipc.h>
#include "server_internal.h"

// Standard Headers
#include <stdbool.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

bool	server_destroy(void)
{
	t_server *const	this = server();

	if (this->subscriptions) delete_indexmap(this->subscriptions);
	*this = (t_server){.socket = -1};
	unlink(SOCKET_PATH);
	return (true);
}

bool	server_init(t_server_handler handler)
{
	t_server *const	this = server();

	if (!handler) return (false);
	if (this->initialized) return (true);

	this->subscriptions = new_indexmap(sizeof(t_pollfd), sizeof(t_poll_event));
	this->subscriptions->custom_keys(this->subscriptions,
									 NULL,
									 pollfd_compare,
									 pollfd_destroy);
	this->request_handler = handler;
	this->initialized = true;
	return (true);
}

bool	server_register_reaction(int fd,
								 void *context,
								 void (*handler)(t_pollfd *pollfd, void *context))
{
	t_server		*const this = server();
	t_pollfd		key;
	t_poll_event	value;

	if (fd < 0 || !handler)
		return (false);
	key = pollfd_init(fd);
	value = (t_poll_event){
		.context = context,
		.handler = handler,
	};
	return (this->subscriptions->set(this->subscriptions,
									 &key,
									 &value));
}

bool	server_unregister_reaction(int fd)
{
	t_server	*const this = server();
	t_pollfd	*found;
	int			index;

	index = this->subscriptions->keys.get_index(&this->subscriptions->keys,
											&(t_pollfd){.fd = fd});
	if (0 > index)
		return (false);
	found = this->subscriptions->keys.at(&this->subscriptions->keys, index);
	found->fd = -1;
	return (this->subscriptions->remove_at(this->subscriptions, index));
}

bool	server_open_connections(t_server *const this)
{
	t_unsock	addr;

	if (this->online)	return (true);

	addr = (t_unsock) { .sun_family = AF_UNIX };
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);
    unlink(SOCKET_PATH);
    if ((this->socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0
		|| bind(this->socket, (struct sockaddr *)&addr, sizeof(addr)) < 0
		|| listen(this->socket, SOMAXCONN) < 0
	) {
		ERROR_SEND;
		goto error;
	}

	server_register_reaction(this->socket, this, server_handle_connections);
	this->online = true;

    return (true);

error:
	if (this->socket >= 0) close(this->socket);
	this->socket = -1;
	unlink(SOCKET_PATH);
	return (false);
}

	// *(t_pollfd*)this->subscriptions->emplace(this->subscriptions) = (t_pollfd) {
	// 	.fd = this->socket,
	// 	.events = POLLIN
	// };
