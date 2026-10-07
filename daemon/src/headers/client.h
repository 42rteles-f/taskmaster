#ifndef CLIENT_H
# define CLIENT_H

# include <poll.h>
# include <ipc.h>
# include <stdbool.h>

typedef struct pollfd t_pollfd;

typedef struct s_client
{
	t_pollfd		pollfd;
	t_ipc_message	message;
}	t_client;

void		client_clean_array(t_uvector *const clients);
bool		client_update(t_client *const this);
void		client_destroy(void *const client);
void		client_clear_data(t_client *const this);
t_client	client_init(int fd);

#endif
