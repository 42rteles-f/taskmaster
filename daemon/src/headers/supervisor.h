#ifndef SUPERVISOR_H
 #define SUPERVISOR_H

 #include <stdbool.h>
 #include <indexmap.h>
 
 typedef struct {
 
 }	t_config;
 
 typedef struct {
	char	*name;
	size_t	id;
 	void	*(*execute)();
 	void	*(*sync)();

 }	t_program;
 
 bool	supervisor_init(void);
 bool	supervisor_update(void);
 bool	supervisor_set_signal(void);
 t_supervisor	*supervisor(void);


#endif