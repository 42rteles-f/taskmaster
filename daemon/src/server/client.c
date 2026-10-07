#include <client.h>

#include <server.h>
#include <poll.h>

void	client_destroy(void *const client)
{
	t_client *const	this = client;

	if (-1 < this->pollfd.fd) close(this->pollfd.fd);
	if (this->message.payload) free(this->message.payload);
	*this = (t_client){0};
}

void	client_clear_data(t_client *const this)
{
	if (this->message.payload) free(this->message.payload);
	this->message.payload = NULL;
	this->message.header = (t_ipc_header){ .type = IPC_EMPTY, .payload_len = 0 };
}

bool	client_update(t_client *const this)
{
	if (!(this->pollfd.revents & POLLIN))
		return (false);
	if (this->pollfd.revents & (POLLHUP | POLLERR) ||
		!ipc_recv(this->pollfd.fd, &this->message.header, &this->message.payload))
	{
		close(this->pollfd.fd);
		this->pollfd.fd = -1;
		return (false);
	}
	return (true);
}

int	client_send(t_client *const this, t_ipc_message *const response)
{
	if (response->type == IPC_EMPTY)
		return (0);
	if (!ipc_send(this->pollfd.fd, &response->header, response->payload))
	{
		close(this->pollfd.fd);
		this->pollfd.fd = -1;
		return (-1);
	}

	return (0);
}

//unorderd vector means last member is copied into removed index
//upon removal, we need to check the same index again.
void	client_clean_array(t_uvector *const clients)
{
	size_t	index;

	index = 0;
	while (index < clients->size)
	{
		if (((t_pollfd*)clients->at(clients, index))->fd < 0)
			clients->remove_at(clients, index);
		else
			index++;
	}
}

void	client_init(int fd)
{
	return ((t_client) {
		.pollfd = { .fd = fd, .events = POLLIN, .revents = 0 },
		.message = ipc_message_init();
	});
}