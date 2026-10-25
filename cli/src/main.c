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

	length = 0;
	for (size_t index = 0; index < argc; index++)
		length += strlen(argv[index]) + 1;

	input = malloc(length);
	if (!input)
		return (NULL);
	input[0] = '\0';

	for (size_t index = 0; index < argc; index++) {
		if (index > 0) 
			strcat(input, " ");
		strcat(input, argv[index]);
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

	if (!terminal_init(this, argc))
	{
		terminal_destroy(this);
		return (this->status);
	}

	if (argc > 1)
	{
		printf("here\n");
		this->input = join_arguments(argc - 1, &(argv[1]));
		process_input(this);
		terminal_destroy(this);
		return (this->status);
	}

	while (this->running)
	{
		catch_input(this);
		if (this->input)
			process_input(this);
	}
	terminal_destroy(this);

	return (this->status);
}
