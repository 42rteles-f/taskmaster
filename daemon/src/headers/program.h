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
	bool		revised;
	t_config	*config;
	t_process	process;

	void	*(*execute)();
	void	*(*sync)();
} t_program;

#endif
