#include <server.h>

bool	server_init()
{
	t_server *const	this = server();

	if (this->clients) delete_vector(this->clients);

	this->clients = new_uvector(sizeof(t_client));
}