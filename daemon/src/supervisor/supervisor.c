#include <supervisor.h>
#include <program.h>
#include 

int	program_name_compare(void *element, void *name)
{
	return (strcmp(((t_program*)element)->name, name));
}

void	program_destroy(t_program *this)
{
	free(this->name);
	//kill process if alive
}

void	call_program_execute(void *element, int)
{
	((t_program*)element)->execute(element);
}

void	set_program_unrevised(void *element, int)
{
	((t_program*)element)->revised = false;
}

bool	supervisor_init(void)
{
	t_supervisor	*this = supervisor();

	if (this->programs)
		delete_vector(this->programs);

	this->programs = new_uvector(sizeof(t_program));
	vector_custom(this->programs, NULL, program_name_compare, program_destroy);
	this->commands = new_indexmap(sizeof(char*), sizeof(t_supervisor_handler));
	this->commands = {
		[CMD_STATUS] = supervisor_handle_status,
		[CMD_START] = supervisor_handle_start,
		[CMD_STOP] = supervisor_handle_stop,
		[CMD_RESTART] = supervisor_handle_restart,
		[CMD_RELOAD] = supervisor_handle_reload,
		[CMD_SHUTDOWN] = supervisor_handle_shutdown
	}
	//start sighandlers
	//start current programs loaded
}

bool	supervisor_update(t_config *config)
{
	t_supervisor	*this = supervisor();
	t_uvector		*new_data;
	t_program		*new_prog;
	t_program		*old_prog;

	//pass config data into supervisor structure
	if (!config)
		printf("Invalid config file. No changes. Not a proper logger");

	new_data = new_uvector(sizeof(t_program));
	vector_custom(this->programs, NULL, program_name_compare, NULL);
	fill_vector(new_data);

	this->programs->for_each(this->programs, set_program_unrevised);
	for (size_t index = 0; index < new_data->size; index++)
	{
		new_prog = new_data->at(new_data, index);
		old_prog = this->programs->find(this->programs, new_prog->name);
		if (!old_prog)
			old_prog = this->programs->push(this->programs, new_prog);
		old_prog->sync(old_prog, new_prog);
		old_prog->revised = true;
	}
	// make for_each_with(), receives an arg to pass along
	// then this loop and the next call can also be for_each calls

	delete_vector(new_data);
	supervisor_execute(this);
}

supervisor_update_programs(t_uvector)

bool	supervisor_set_signals(void)
{
	t_supervisor	*this = supervisor();
	
}

bool	supervisor_execute(t_supervisor *this, t_uvector programs)
{
	
}

void	supervisor_destroy(void)
{
	t_supervisor	*this = supervisor();

	delete_vector(this->programs);
}

//programs are 