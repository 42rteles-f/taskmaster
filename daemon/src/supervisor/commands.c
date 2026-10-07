#include <supervisor.h>
#include <server.h>

t_ipc_message	supervisor_handle_request(const t_ipc_message *const req)
{
	t_ipc_message	response;
	t_supervisor_handler	handler;

	if (IPC_EMPTY == req->header.type || IPC_COUNT <= req->header.type) {
		return (ipc_message_init());
	}
	handler = supervisor()->commands[req->header.type];
	return (handler(req->header.payload_len, req->payload));
}


void	supervisor_handle_status(const t_ipc_message *req)
{

}

void	supervisor_handle_start(const t_ipc_message *req)
{

}

void supervisor_handle_stop(const t_ipc_message *req)
{

}

void supervisor_handle_restart(const t_ipc_message *req)
{

}

void supervisor_handle_reload(const t_ipc_message *req)
{

}

void supervisor_handle_shutdown(const t_ipc_message *req)
{

}

