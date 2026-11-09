#ifndef PROGRAM_H
# define PROGRAM_H

// Custom Headers
# include <config.h>
# include <vector.h>

// Standard Headers
# include <stdbool.h>
# include <stddef.h>

typedef enum {
	PROGRAM_STOPPED,
	PROGRAM_HEALTHY,
	PROGRAM_DEGRADED,
	PROGRAM_FAILED,
	PROGRAM_STOPPING,
	PROGRAM_STARTING,
	PROGRAM_RESTARTING,
	PROGRAM_STATE_COUNT,
} t_program_state;

typedef struct s_program t_program;

struct s_program {
	size_t			id;
	size_t			next_slot_id;
	t_program_state	state;
	size_t			fatal_childs;
	bool			revised;
	t_config		*config;
	t_uovector		processes;

	void	*(*execute)();
	void	(*sync)(t_program *this, t_config *config);
	void	(*update)(t_program *this);
	void	(*destroy)(void *this);
};

t_program	program_init(void);
void		program_destroy(void *element);
void		program_sync(t_program *this, t_config *new_config);
void		program_update(t_program *this, struct timespec *now);
int			program_cmp_str_name(const void *element, const void *name, size_t size);
void		program_handle_child_exit(t_program *this, pid_t pid, int status, struct timespec *now);

#endif
