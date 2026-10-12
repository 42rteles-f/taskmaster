#include <terminal.h>

#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

t_control	*terminal(void)
{
	static t_control	this;

	return (&this);
}

static void	catch_input(t_control *this)
{
	size_t	capacity;

	if (!this->interactive)
	{
		capacity = 0;
		if (getline(&this->input, &capacity, stdin) < 0)
		{
			free(this->input);
			this->input = NULL;
			control_d(this);
		}
		return ;
	}
	this->input = readline(TERMINAL_PROMPT);
	if (!this->input)
		control_d(this);
	else if (*this->input)
		add_history(this->input);
}

static char	*join_arguments(int argc, char **argv)
{
	size_t	length;
	char	*input;
	int		index;

	length = 0;
	index = 1;
	while (index < argc)
		length += strlen(argv[index++]) + 1;
	input = malloc(length);
	if (!input)
		return (NULL);
	*input = '\0';
	index = 1;
	while (index < argc)
	{
		if (index > 1)
			strcat(input, " ");
		strcat(input, argv[index++]);
	}
	return (input);
}

static void	process_input(t_control *this)
{
	if (normalize_input(this) && structure_commands(this))
		run_input(this);
	input_reset(this);
}

int	main(int argc, char **argv)
{
	t_control	*const this = terminal();

	if (!setup(this, argc == 1 && isatty(STDIN_FILENO)))
	{
		end_terminal(this);
		return (this->status);
	}
	if (argc > 1)
	{
		this->input = join_arguments(argc, argv);
		if (!this->input)
			this->status = EXIT_FAILURE;
		else
			process_input(this);
	}
	else
	{
		while (this->running)
		{
			catch_input(this);
			if (this->input)
				process_input(this);
		}
	}
	end_terminal(this);
	return (this->status);
}
