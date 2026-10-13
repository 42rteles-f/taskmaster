// Custom Headers
#include <server.h>
#include "server_internal.h"
// Standard Headers
#include <poll.h>
#include <stddef.h>

t_pollfd	*new_pollfd(size_t fd)
{
	t_pollfd	*const new = malloc(sizeof(t_pollfd));

	*new = (t_pollfd){ .fd = fd, .events = POLLIN, .revents = 0 };
	return (new);
}

int	pollfd_compare(const void *const left, const void *const right, const size_t size)
{
	(void)size;
	return (((t_pollfd*)left)->fd - ((t_pollfd*)right)->fd);
}

void	delete_pollfd(t_pollfd *const this)
{
	if (-1 < this->fd) close(this->fd);
	*this = (t_pollfd){};
	free(this);
}
