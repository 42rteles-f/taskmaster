#ifndef SERVER_INTERNAL_H
# define SERVER_INTERNAL_H

# include <server.h>

void		server_handle_connections(void *const arg);
void		server_handle_pollable(void *const arg);
bool		server_open_connections(t_server *const this);

t_pollfd	pollfd_init(int fd);
int			pollfd_compare(const void *const left, const void *const right,
				size_t size);
void		pollfd_destroy(void *arg);

#endif
