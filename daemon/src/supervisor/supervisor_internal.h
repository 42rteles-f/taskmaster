#ifndef SUPERVISOR_INTERNAL_H
# define SUPERVISOR_INTERNAL_H

// Custom Headers
#include <ipc.h>

// Standard Headers
#include <stdbool.h>

t_ipc_message	supervisor_handle_request(const t_ipc_message *const request);
t_ipc_message	supervisor_handle_status(const t_ipc_message *req);
t_ipc_message	supervisor_handle_start(const t_ipc_message *req);
t_ipc_message	supervisor_handle_stop(const t_ipc_message *req);
t_ipc_message	supervisor_handle_restart(const t_ipc_message *req);
t_ipc_message	supervisor_handle_reload(const t_ipc_message *req);
t_ipc_message	supervisor_handle_shutdown(const t_ipc_message *req);
bool			supervisor_set_signals(void);
bool			supervisor_setup_pipe(void);

#endif
