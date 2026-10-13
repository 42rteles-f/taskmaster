#ifndef SERVER_INTERNAL_H
# define SERVER_INTERNAL_H

# include <server.h>

void	    server_handle_connections(void *const arg);

t_pollfd	*new_pollfd(size_t fd);
int	        pollfd_compare(const void *const left, const void *const right, const size_t size);
void	    delete_pollfd(t_pollfd *const this);

#endif