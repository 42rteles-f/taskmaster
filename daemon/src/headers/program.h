#ifndef PROGRAM_H
# define PROGRAM_H

// Custom Headers
# include <config.h>
# include <vector.h>

// Standard Headers
# include <stdbool.h>
# include <stddef.h>

typedef struct s_program t_program;

struct s_program {
	size_t		id;
	size_t		next_slot_id;
	bool		revised;
	t_config	config;
	t_uovector	processes;

	void	*(*execute)();
	void	(*update)(t_program *this, t_config *config);
	void	*(*sync)();
	void	(*destroy)(void *this);
};

t_program	program_init(void);
void		program_destroy(void *element);
void		program_update(t_program *this, t_config *new_config);
int			program_cmp_str_name(const void *element, const void *name, size_t size);

#endif
