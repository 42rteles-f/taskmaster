#ifndef PROGRAM_H
 #define PROGRAM_H
 
 #include <stdio.h>
 
 typedef struct {
	char	*name;
	size_t	id;
    bool    revised;

 	void	*(*execute)();
 	void	*(*sync)();
 }	t_program;

#endif