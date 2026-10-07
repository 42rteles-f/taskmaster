#ifndef SUPERVISOR_H
# define SUPERVISOR_H

# include <stdbool.h>
# include <indexmap.h>
# include <ipc.h>

typedef void	(*t_supervisor_handler)(int, void*);

typedef struct {
	t_vector				*programs;
	t_supervisor_handler	*commands[CMD_COUNT];

	void	*(*init)();
	void	*(*sync)();
	void	*(*shutdown)();
	void	(*handleRequest)();
	void	(*destroy)();
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