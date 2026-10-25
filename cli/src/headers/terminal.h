#ifndef TERMINAL_H
# define TERMINAL_H

# include <stdbool.h>
# include <ipc.h>

# define TERMINAL_PROMPT "taskmaster> "

typedef struct s_control	t_control;
typedef struct s_command	t_command;

struct s_command
{
	t_ipc_message	message;
	t_control		*control;
	void			(*execute)(t_command *this);
};

struct s_control
{
	char				*input;
	char				**split;
	int					socket;
	int					status;
	bool				running;
	t_command			command;
};

t_control	*terminal(void);
bool		terminal_init(t_control *this, int argc);
void		control_d(t_control *this);
bool		normalize_input(t_control *this);
bool		structure_commands(t_control *this);
void		run_input(t_control *this);
void		input_reset(t_control *this);
void		terminal_destroy(t_control *this);

#endif
