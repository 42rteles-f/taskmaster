#ifndef SUPERVISOR_H
# define SUPERVISOR_H

// Custom Headers
# include <config.h>
# include <ipc.h>
# include <vector.h>

// Standard Headers
# include <stdbool.h>
# include <poll.h>

#define SUPERVISOR_CONFIG_FILE	"taskmaster.yaml"

typedef t_ipc_message	(*t_supervisor_handler)(const t_ipc_message *);

typedef struct {
	int						signal_pipe[2];
	int						timer_fd;
	t_uovector				*programs;
	t_supervisor_handler	commands[IPC_COUNT];

	bool	(*init)(void);
	bool	(*start)();
	bool	(*update)();
	bool	(*sync)(t_uovector **);
	bool	(*new_program)(t_config *);
	void	(*shutdown)(void);
	void	(*destroy)(void);

	t_ipc_message	(*handle_request)(const t_ipc_message *const);
}	t_supervisor;

bool			supervisor_init(void);
// bool			supervisor_update(void);
bool			supervisor_sync(t_uovector **configs);
void			supervisor_destroy(void);
void			supervisor_handle_pipe(struct pollfd *pollfd, void *context);
void			supervisor_handle_timer(struct pollfd *pollfd, void *context);
t_ipc_message	supervisor_handle_request(const t_ipc_message *const request);

t_supervisor	*supervisor(void);

#endif
