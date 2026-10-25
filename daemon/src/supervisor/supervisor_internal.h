#ifndef SUPERVISOR_INTERNAL_H
# define SUPERVISOR_INTERNAL_H

#include <ipc.h>

t_ipc_message	supervisor_handle_request(const t_ipc_message *const request);
t_ipc_message	supervisor_handle_status(const t_ipc_message *req);
t_ipc_message	supervisor_handle_start(const t_ipc_message *req);
t_ipc_message	supervisor_handle_stop(const t_ipc_message *req);
t_ipc_message	supervisor_handle_restart(const t_ipc_message *req);
t_ipc_message	supervisor_handle_reload(const t_ipc_message *req);
t_ipc_message	supervisor_handle_shutdown(const t_ipc_message *req);

#endif