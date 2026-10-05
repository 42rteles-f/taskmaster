#ifndef SUPERVISOR_H
 #define SUPERVISOR_H

 #include <stdbool.h>
 #include <indexmap.h>

 typedef struct {
	t_indexmap	*programs;

 	void	*(*init)();
 	void	*(*setup)();
 	void	*(*update)();
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