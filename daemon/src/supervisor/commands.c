#include <supervisor.h>
#include <server.h>

static t_ipc_message	invalid_response(void)
{
	t_ipc_message	response;

	response = ipc_message_init();
	ipc_message_set(&response, IPC_INVALID, "invalid request",
		sizeof("invalid request"));
	return (response);
}

t_ipc_message	supervisor_handle_request(const t_ipc_message *const request)
{
	t_supervisor_handler	handler;

	if (IPC_EMPTY == request->header.type || IPC_COUNT <= request->header.type) {
		return (invalid_response());
	}
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

