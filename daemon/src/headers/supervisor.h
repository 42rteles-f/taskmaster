#ifndef SUPERVISOR_H
 #define SUPERVISOR_H


typedef struct {
	void	*(*init)();
	void	*(*setup)();
	void	*(*update)();
}	t_supervisor;

typedef struct {

}	t_config;

typedef struct {

}	t_program;

#endif