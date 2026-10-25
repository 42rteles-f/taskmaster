#include <supervisor.h>
#include <server.h>

t_ipc_message	supervisor_handle_request(const t_ipc_message *const request)
{
	t_supervisor_handler	handler;

	printf("handling check\n");

	if (IPC_EMPTY == request->header.type || IPC_COUNT <= request->header.type) {
		return (ipc_message_init());
	}
	printf("handling pass\n");
	handler = supervisor()->commands[request->header.type];
	return (handler(request));
}


t_ipc_message	supervisor_handle_status(const t_ipc_message *req)
{
	printf("status\n");
}

t_ipc_message	supervisor_handle_start(const t_ipc_message *req)
{

}

t_ipc_message	supervisor_handle_stop(const t_ipc_message *req)
{
	printf("stopping server\n");
	server()->online = false;
}

t_ipc_message	supervisor_handle_restart(const t_ipc_message *req)
{

}

t_ipc_message	supervisor_handle_reload(const t_ipc_message *req)
{

}

t_ipc_message	supervisor_handle_shutdown(const t_ipc_message *req)
{

}

