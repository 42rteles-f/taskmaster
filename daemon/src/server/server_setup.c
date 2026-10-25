// Custom Headers
#include <server.h>
#include <ipc.h>
#include "server_internal.h"
#include "pollable.h"
// Standard Headers
#include <poll.h>
#include <sys/socket.h>
#include <errno.h>

bool	server_destroy(void)
{
	t_server *const	this = server();

	if (this->pollables) delete_indexmap(this->pollables);
	*this = (t_server){.socket = -1};
	return (true);
}

bool	server_init(void)
{
	t_server *const	this = server();

	if (this->initialized) return (true);

	this->pollables = new_indexmap(sizeof(t_pollfd), sizeof(t_pollable));
	this->pollables->custom_keys(this->pollables, NULL, pollfd_compare, pollfd_destroy);
	this->pollables->custom_values(this->pollables, NULL, NULL, pollable_destroy);
	this->initialized = true;
	return (true);
}

void	server_set_socket(t_server *const this, int socket)
{
	t_pollable	poll_obj;
	t_pollfd	pollfd;

	if (this->socket >= 0)
		this->pollables->remove(this->pollables,
			&(t_pollfd){.fd = this->socket});
	pollfd = pollfd_init(socket);
	poll_obj = pollable_init();
	poll_obj.handler = server_handle_connections;
	poll_obj.context = this;
	this->socket = socket;
	this->pollables->set(this->pollables, &pollfd, &poll_obj);
}

bool	server_open_connections(t_server *const this)
{
	t_unsock	addr;
	int			fd;

	if (this->online)	return (true);

	fd = -1;
	addr = (t_unsock) { .sun_family = AF_UNIX };
	snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);
    unlink(SOCKET_PATH);
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0
		|| bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0
		|| listen(fd, SOMAXCONN) < 0
	) {
		ERROR_SEND;
		goto error;
	}

	server_set_socket(this, fd);
	this->online = true;

    return (true);

error:
	if (fd >= 0) close(fd);
	this->socket = -1;
	unlink(SOCKET_PATH);
	return (false);

}

	// *(t_pollfd*)this->pollables->emplace(this->pollables) = (t_pollfd) {
	// 	.fd = this->socket,
	// 	.events = POLLIN
	// };
