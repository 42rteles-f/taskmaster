#ifndef PROCESS_H
# define PROCESS_H

// Custom Headers
# include <config.h>

// Standard Headers
# include <stdbool.h>
# include <stddef.h>
# include <sys/types.h>
# include <time.h>

typedef enum e_process_state {
	PROCESS_STOPPED,
	PROCESS_STARTING,
	PROCESS_RUNNING,
	PROCESS_STOPPING,
	PROCESS_BACKOFF,
	PROCESS_EXTERMINATING,
	PROCESS_FATAL
} t_process_state;

typedef struct s_process {
	size_t				slot_id;
	const t_config		*config;
	pid_t				pid;
	int					wait_status;
	unsigned int		retries;
	t_process_state		state;
	struct timespec		state_at;
	struct timespec		deadline;
} t_process;

t_process	process_init(size_t slot_id, const t_config *config);
bool		process_start(t_process *process);
void		process_stop(t_process *process);
void		process_update(t_process *process, struct timespec *now);
void		process_destroy(void *element);
int			process_compare_id(const void *first, const void *second, size_t size);
void		process_force_stop(t_process *this);


#endif
