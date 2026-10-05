#ifndef TERMINAL_H
 #define TERMINAL_H

typedef struct {
	void	*(*setup)();
	void	*(*init)();
}	t_terminal;

#endif