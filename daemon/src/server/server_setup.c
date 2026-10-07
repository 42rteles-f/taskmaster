#include <server.h>
#include <ipc.h>
#include <client.h>

#include <poll.h>
#include <sys/socket.h>
#include <errno.h>

bool	server_destroy(void)
{
	t_server *const	this = server();

	if (this->clients) delete_vector(this->clients);
	if (-1 < this->socket) close(this->socket);
	*this = (t_server){0};
	return (true);
}

void	client_destroy(void *const client)
{
	t_pollfd *const	pollfd = (t_pollfd*)client;

	if (pollfd->fd > -1)
		close(pollfd->fd);
}

bool	server_init(void)
{
	t_server *const	this = server();

	if (this->initialized) return (true);

	this->clients = new_uvector(sizeof(t_client));
	vector_custom(this->clients, NULL, NULL, client_destroy);

	this->initialized = true;
	return (true);
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
        return (false);
	}

	*(t_pollfd*)this->clients->emplace(this->clients) = (t_pollfd) {
		.fd = this->socket,
		.events = POLLIN
	};
	this->online = true;

    return (true);

error:
	if (this->socket >= 0)
		close(this->socket);
	this->socket = -1;
	unlink(SOCKET_PATH);
	return (false);

}