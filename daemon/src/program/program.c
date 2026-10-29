#include <program.h>

t_program program_init()
{
	return ((t_program){});
}

int	program_name_compare(const void *element, const void *name, size_t size)
{
	(void)size;
	return (strcmp(((t_program*)element)->name, name));
}

void	program_destroy(void *arg)
{
	t_program	*const this = arg;
	free(this->name);
	//kill process if alive
}