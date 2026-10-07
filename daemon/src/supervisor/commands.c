#include <supervisor.h>
#include <server.h>

		[CMD_STATUS] = supervisor_handle_status,
		[CMD_START] = supervisor_handle_start,
		[CMD_STOP] = supervisor_handle_stop,
		[CMD_RESTART] = supervisor_handle_restart,
		[CMD_RELOAD] = supervisor_handle_reload,
		[CMD_SHUTDOWN] = supervisor_handle_shutdown

void	supervisor_handle_request(const t_ipc_header *header, void **payload)
{
	if (req->command >= CMD_COUNT)
	{
		t_response res = { .status = 1, .payload_len = 0 };
		ipc_send(fd, &res, NULL);
		return;
	}
	supervisor()->command_handlers[req->command](req, payload);
}

void	supervisor_handle_start(const t_request *req, void **payload)
{

}

void supervisor_handle_stop(const t_request *req, void **payload)
{

}

void supervisor_handle_restart(const t_request *req, void **payload)
{

}

void supervisor_handle_reload(const t_request *req, void **payload)
{

}

void supervisor_handle_shutdown(const t_request *req, void **payload)
{

}
