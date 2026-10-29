#ifndef PROGRAM_H
# define PROGRAM_H

# include <stdbool.h>
# include <stddef.h>
# include <indexmap.h>

#include "config.h"

typedef	struct {

}	t_process;

typedef struct s_program {
	size_t		id;
	char		*name;
	bool		revised;
	t_config	*config;
	t_uovector	*process;

	void	*(*execute)();
	void	*(*update)();
	void	*(*sync)();
} t_program;

t_program	program_init(void);
void		program_destroy(void *element);
int			program_name_compare(const void *element, const void *name, size_t size);

#endif
