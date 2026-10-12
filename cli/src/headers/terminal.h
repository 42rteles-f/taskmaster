#ifndef TERMINAL_H
# define TERMINAL_H

# include <stdbool.h>
# include <ipc.h>

# define TERMINAL_PROMPT "taskmaster> "

typedef enum e_terminal_action
{
	TERMINAL_REQUEST,
	TERMINAL_HELP,
	TERMINAL_EXIT
}	t_terminal_action;

typedef struct s_control
{
	char				*input;
	char				*payload;
	int					socket;
	int					status;
	bool				interactive;
	bool				running;
	t_ipc_type			command;
	t_terminal_action	action;
}	t_control;

t_control	*terminal(void);
bool		setup(t_control *this, bool interactive);
void		control_d(t_control *this);
bool		normalize_input(t_control *this);
bool		structure_commands(t_control *this);
void		run_input(t_control *this);
void		input_reset(t_control *this);
void		end_terminal(t_control *this);

#endif
