#ifndef TERMINAL_INTERNAL_H
# define TERMINAL_INTERNAL_H

# include <terminal.h>

typedef bool	(*t_command_builder)(t_control *this);

char	**ft_split_space(const char *string);
void	*free_split(char **split);

void	command_execute_request(t_command *this);
void	command_execute_help(t_command *this);
void	command_execute_exit(t_command *this);

#endif
