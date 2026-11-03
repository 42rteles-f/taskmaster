#ifndef SERVER_INTERNAL_H
# define SERVER_INTERNAL_H

// Custom Headers
# include <server.h>

// Standard Headers
# include <stddef.h>

typedef struct s_poll_subscription
{
	void	*context;
	void	(*handler)(t_pollfd *pollfd, void *context);
} t_poll_event;

void		server_handle_connections(t_pollfd *pollfd, void *context);
bool		server_open_connections(t_server *const this);

t_pollfd	pollfd_init(int fd);
int			pollfd_compare(const void *const left, const void *const right,
				size_t size);
void		pollfd_destroy(void *arg);

#endif
