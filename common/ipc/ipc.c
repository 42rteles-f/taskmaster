#include "ipc.h"
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <stddef.h>

int ipc_transfer_all(int fd, const void *buf, size_t len)
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

int recv_all(int fd, void *buf, size_t len)
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

int ipc_send(int fd, const t_response *res, const void *payload)
{
    if (send_all(fd, res, sizeof(*res)) < 0)
        return (-1);

    if (res->payload_len > 0)
    {
        if (!payload)
            return (-1);

        if (send_all(fd, payload, res->payload_len) < 0)
            return (-1);
    }

    return (0);
}

int ipc_recv(int fd, t_response *res, void **payload)
{
    if (recv_all(fd, res, sizeof(*res)) < 0)
        return (-1);
    if (res->payload_len == 0)
        return (0);

    *payload = malloc(res->payload_len);
    if (!*payload)
        return (-1);

    if (recv_all(fd, *payload, res->payload_len) < 0)
    {
        free(*payload);
        *payload = NULL;
        return (-1);
    }

    return (0);
}
