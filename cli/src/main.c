#include "./headers/terminal.h"

t_terminal	*terminal()
{
	static	t_terminal	cli = {

	};
	return (&cli);
}

int	main(int argc, char **argv)
{
	terminal()->setup();
	terminal()->init();
}