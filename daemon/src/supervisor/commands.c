#include <supervisor.h>
#include <server.h>

t_ipc_message	supervisor_handle_request(const t_ipc_message *const request)
{
	t_ipc_message			response;
	t_supervisor_handler	handler;

	if (IPC_EMPTY == request->header.type || IPC_COUNT <= request->header.type) {
		return (ipc_message_init());
	}
	handler = supervisor()->commands[request->header.type];
	return (handler(request->header.payload_len, request->payload));
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

