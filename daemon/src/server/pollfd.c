// Custom Headers
#include <server.h>
#include "server_internal.h"

t_pollfd	pollfd_init(int fd)
{
	return ((t_pollfd){.fd = fd, .events = POLLIN});
}

int	pollfd_compare(const void *const left, const void *const right, const size_t size)
{
	(void)size;
	return (((const t_pollfd *)left)->fd - ((const t_pollfd *)right)->fd);
}

void	pollfd_destroy(void *arg)
{
	t_pollfd *const	pollfd = arg;

	if (pollfd->fd >= 0)
		close(pollfd->fd);
	pollfd->fd = -1;
}
