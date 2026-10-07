#ifndef PROTOCOL_H
# define PROTOCOL_H

# include <stdint.h>

typedef enum e_command
{
    CMD_STATUS,
    CMD_START,
    CMD_STOP,
    CMD_RESTART,
    CMD_RELOAD,
    CMD_SHUTDOWN,
	CMD_COUNT
} t_command;

typedef struct s_request
{
    uint32_t command;
    uint32_t payload_len;
} t_request;

typedef struct s_response
{
    uint32_t status;
    uint32_t payload_len;
} t_response;

int ipc_send(int fd, const t_response *res, const void *payload);
int ipc_recv(int fd, t_response *res, void **payload);

#endif