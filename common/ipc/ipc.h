#ifndef IPC_H
# define IPC_H

# include <stdint.h>

# define SOCKET_PATH		"/tmp/taskmaster.sock"
# define IPC_TIMEOUT_SEC	3
# define IPC_MAX_PAYLOAD 	(64 * 1024)

typedef enum e_ipc_type
{
    IPC_STATUS,
    IPC_START,
    IPC_STOP,
    IPC_RESTART,
    IPC_RELOAD,
    IPC_SHUTDOWN,
	IPC_COUNT
} t_ipc_type;

typedef struct s_ipc_header
{
    uint32_t type;
    uint32_t payload_len;
} t_ipc_header;

int ipc_send(int fd, const t_ipc_header *res, const void *payload);
int ipc_recv(int fd, t_ipc_header *res, void **payload);
int ipc_accept(const int server_fd);
int	ipc_connect(void);

#endif