#include <server.h>
#include <ipc.h>
#include <client.h>
#include "pollfd.c"

#include <poll.h>
#include <sys/socket.h>
#include <errno.h>

bool	server_destroy(void)
{
	t_server *const	this = server();

	if (this->pollables) delete_vector(this->pollables);
	if (-1 < this->socket) close(this->socket);
	*this = (t_server){0};
	return (true);
}

bool	server_init(void)
{
	t_server *const	this = server();

	if (this->initialized) return (true);

	this->pollables = new_indexmap(sizeof(t_pollfd*), sizeof(t_pollable));
	this->pollables->custom_keys(this->pollables, NULL, pollfd_compare, NULL);
	this->pollables->custom_values(this->pollables, NULL, NULL, pollable_destroy);

	this->initialized = true;
	return (true);
}

bool	server_open_connections(t_server *const this)
{
	t_unsock	addr;
	t_pollable	server;

	if (this->online)	return (true);

	addr = (t_unsock) { .sun_family = AF_UNIX };
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);
    unlink(SOCKET_PATH);
    if ((this->socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0
		|| bind(this->socket, (struct sockaddr *)&addr, sizeof(addr)) < 0
		|| listen(this->socket, SOMAXCONN) < 0
	) {
		ERROR_SEND;
        return (false);
	}

	server = pollable_init(this->socket);
	server.handler = server_handle_connections;
	this->pollables->set(this->pollables, server.pollfd, &server);
	this->online = true;

    return (true);

error:
	if (this->socket >= 0) close(this->socket);
	this->socket = -1;
	unlink(SOCKET_PATH);
	return (false);

}

	// *(t_pollfd*)this->pollables->emplace(this->pollables) = (t_pollfd) {
	// 	.fd = this->socket,
	// 	.events = POLLIN
	// };