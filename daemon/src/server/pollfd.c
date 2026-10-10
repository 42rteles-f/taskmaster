#include <server.h>

#include <poll.h>
#include <stddef.h>

t_pollfd	*new_pollfd(size_t fd)
{
	t_pollfd	*const new = malloc(sizeof(t_pollfd));

	*new = (t_pollfd){ .fd = fd, .events = POLLIN, .revents = 0 };
	return (new);
}

int	pollfd_compare(const void *left, const void *right, size_t size)
{
	return (((t_pollfd*)left)->fd - ((t_pollfd*)right)->fd);
}

void	delete_pollfd(const t_pollfd *this)
{
	if (-1 < this->fd) close(this->fd);
	free(this);
}
