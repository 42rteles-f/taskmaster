#ifndef SUPERVISOR_H
# define SUPERVISOR_H

# include <stdbool.h>
# include <indexmap.h>
# include <ipc.h>
# include <signal.h>

#define SUPERVISOR_CONFIG_FILE	"taskmaster.yaml"

typedef t_ipc_message	(*t_supervisor_handler)(int, void*);

typedef struct {
	volatile sig_atomic_t	signal;
	t_vector				*programs;
	t_supervisor_handler	commands[IPC_COUNT];

	void	*(*init)();
	void	*(*update)();
	void	*(*sync)();
	void	*(*shutdown)();
	void	(*destroy)();

	t_ipc_message	(*handle_request)(t_ipc_message *const);
}	t_supervisor;

typedef struct {

}	t_config;
 
typedef struct {

}	t_program;
 
bool	supervisor_init(void);
bool	supervisor_update(void);
bool	supervisor_set_signal(void);
t_supervisor	*supervisor(void);

#endif