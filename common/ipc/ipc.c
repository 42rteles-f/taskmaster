#include "ipc.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <stddef.h>
#include <sys/un.h>
#include <sys/time.h>
#include <unistd.h>

static int send_all(int fd, const void *buf, size_t len)
{
    const char  *ptr;
    ssize_t     sent;

    ptr = buf;
    while (len > 0)
    {
        sent = send(fd, ptr, len, 0);
        if (sent < 0)
        {
            if (errno == EINTR)
                continue;
            return (-1);
        }
        if (sent == 0)
            return (-1);

        ptr += sent;
        len -= sent;
    }
    return (0);
}

static int recv_all(int fd, void *buf, size_t len)
{
    char    *ptr;
    ssize_t received;

    ptr = buf;
    while (len > 0)
    {
        received = recv(fd, ptr, len, 0);
        if (received < 0)
        {
            if (errno == EINTR)
                continue;
            return (-1);
        }
        if (received == 0)
            return (-1);

        ptr += received;
        len -= received;
    }
    return (0);
}

int ipc_send(int fd, const t_ipc_header *header, const void *payload)
{
	if (IPC_MAX_PAYLOAD < header->payload_len)
		return (-1);
    if (send_all(fd, header, sizeof(*header)) < 0)
        return (-1);

    if (header->payload_len > 0)
    {
        if (!payload || send_all(fd, payload, header->payload_len) < 0)
            return (-1);
    }

    return (0);
}

int ipc_recv(int fd, t_ipc_header *header, void **payload)
{
	*payload = NULL;
    if (recv_all(fd, header, sizeof(*header)) < 0)
        return (-1);
	if (IPC_MAX_PAYLOAD < header->payload_len)
		return (-1);
    if (header->payload_len == 0)
        return (0);

    *payload = malloc(header->payload_len);
    if (!*payload)
        return (-1);

    if (recv_all(fd, *payload, header->payload_len) < 0)
    {
        free(*payload);
        *payload = NULL;
        return (-1);
    }

    return (0);
}

int ipc_accept(const int server_fd)
{
	const struct timeval	timeout = { .tv_sec = IPC_TIMEOUT_SEC, .tv_usec = 0 };
	const int 				client_fd = accept(server_fd, NULL, NULL);

	if (client_fd < 0)
		return (-1);

	if (
		setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO,
			&timeout, sizeof(timeout)) < 0
		|| setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO,
			&timeout, sizeof(timeout)) < 0
	) {
		close(client_fd);
		return (-1);
	}
	
	return (client_fd);
}

int	ipc_connect(void)
{
	const int					fd = socket(AF_UNIX, SOCK_STREAM, 0);
	const struct sockaddr_un	addr = {
		.sun_family = AF_UNIX,
		.sun_path = SOCKET_PATH
	};
	const struct timeval		timeout = {
		.tv_sec = IPC_TIMEOUT_SEC,
		.tv_usec = 0
	};

	if (fd < 0)
		return (-1);

	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO,
			&timeout, sizeof(timeout)) < 0
		|| setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO,
			&timeout, sizeof(timeout)) < 0
		|| connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0
	) {
		close(fd);
		return (-1);
	}

	return (fd);
}

int	ipc_create_server(int *const server_fd, struct sockaddr_un *const addr)
{
	*addr = (struct sockaddr_un) { .sun_family = AF_UNIX };
	snprintf(addr->sun_path, sizeof(addr->sun_path), "%s", SOCKET_PATH);
    unlink(addr->sun_path);
	*server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

	if (*server_fd < 0
		|| bind(*server_fd, (struct sockaddr *)addr, sizeof(*addr)) < 0
		|| listen(*server_fd, SOMAXCONN) < 0
	) {
		return (-1);
	}
	return (0);
}

t_ipc_message	ipc_message_init(void *const payload)
{
	return ((t_ipc_message) {
		.header = { 
			.type = IPC_EMPTY,
			.payload_len = 0
		},
		.payload = payload
	});
}

void ipc_message_destroy(t_ipc_message *const message)
{
	if (message->payload) free(message->payload);

	*message = (t_ipc_message) {
		.header = { 
			.type = IPC_EMPTY,
			.payload_len = 0
		},
		.payload = NULL
	};
}
