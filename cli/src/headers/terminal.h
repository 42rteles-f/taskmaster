#ifndef TERMINAL_H
# define TERMINAL_H

# include <stdbool.h>
# include <ipc.h>

# define TERMINAL_PROMPT "taskmaster> "

typedef struct s_control	t_control;
typedef bool				(*t_exe)(t_control *this);

typedef enum e_terminal_action
{
	TERMINAL_REQUEST,
	TERMINAL_HELP,
	TERMINAL_EXIT
}	t_terminal_action;

struct s_control
{
	char				*input;
	char				**split;
	int					socket;
	int					status;
	bool				running;
	t_ipc_message		message;
	t_terminal_action	action;
};

t_control	*terminal(void);
bool		terminal_setup(t_control *this, int argc);
void		control_d(t_control *this);
bool		normalize_input(t_control *this);
bool		structure_commands(t_control *this);
void		run_input(t_control *this);
void		input_reset(t_control *this);
void		terminal_destroy(t_control *this);

#endif
