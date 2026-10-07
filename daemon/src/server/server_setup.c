#include <server.h>
#include <poll.h>
#include <sys/socket.h>
#include <errno.h>
#include <ipc.h>

bool	server_destroy(void)
{
	t_server *const	this = server();

	if (this->clients) delete_vector(this->clients);
	this->initialized = false;
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

	this->clients = new_uvector(sizeof(t_pollfd));
	vector_customize(this->clients, NULL, NULL, client_destroy);

	this->server_addr = (t_unsock) { .sun_family = AF_UNIX };
	snprintf(this->server_addr.sun_path,
			 sizeof(this->server_addr.sun_path),
			 "%s", SOCKET_PATH);

	this->initialized = true;
	return (true);
}

bool	server_open_connections(t_server *const this)
{
    if (!this->initialized) server_init();
	if (this->online)	return (true);

    unlink(this->server_addr.sun_path);
    if ((this->socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0
		|| bind(
        	this->socket,
        	(struct sockaddr *)&this->server_addr,
        	sizeof(this->server_addr)
		) < 0
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
	unlink(this->server_addr.sun_path);
	return (false);

}