#include <supervisor.h>
#include <program.h>
#include <ipc.h>
#include "supervisor_internal.h"

t_supervisor	*supervisor(void)
{
	static t_supervisor	this = {
		.init = supervisor_init,
		.handle_request = supervisor_handle_request,
		.sync = supervisor_sync,
		.destroy = supervisor_destroy
	};

	return (&this);
}

bool	supervisor_init(void)
{
	t_supervisor	*this = supervisor();

	this->programs = new_uovector(sizeof(t_program));
	vector_custom(this->programs, NULL, program_name_compare, program_destroy);

	this->commands[IPC_STATUS] = supervisor_handle_status;
	this->commands[IPC_START] = supervisor_handle_start;
	this->commands[IPC_STOP] = supervisor_handle_stop;
	this->commands[IPC_RESTART] = supervisor_handle_restart;
	this->commands[IPC_RELOAD] = supervisor_handle_reload;
	this->commands[IPC_SHUTDOWN] = supervisor_handle_shutdown;

	//start sighandlers
}

void	supervisor_destroy(void)
{
	t_supervisor	*this = supervisor();

	delete_vector(this->programs);
}

void	call_program_execute(void *element, int)
{
	((t_program*)element)->execute(element);
}

void	set_program_unrevised(void *element, size_t)
{
	((t_program*)element)->revised = false;
}

bool	supervisor_set_signals(void)
{
	t_supervisor	*this = supervisor();
	
}

static void	supervisor_remove_unrevised(t_supervisor *this)
{
	size_t		index;
	t_program	*program;

	index = 0;
	while (index < this->programs->size)
	{
		program = this->programs->at(this->programs, index);
		if (false == program->revised) {
			printf("removing program: %s\n", program->name);
			this->programs->remove_at(this->programs, index);
		}
		else
			index++;
	}
}

void	supervisor_update_program(void	*config, size_t size)
{
	t_uovector *const	_programs = supervisor()->programs;
	t_program			*program;

	(void)size;
	program = _programs->find(_programs, ((t_config*)config)->name);
	if (!program) {
		program = _programs->emplace(_programs);
		*program = program_init();
	}

	program->update(config);
	program->revised = true;
}

bool	supervisor_sync(t_uovector *configs)
{
	t_supervisor *const this = supervisor();

	if (!configs) return (false);

	this->programs->for_each(this->programs, set_program_unrevised);
	configs->for_each(configs, supervisor_update_program);
	supervisor_remove_unrevised(this);
}


	// for (size_t index = 0; index < config_vec->size; index++)
	// {
	// 	config = config_vec->at(config_vec, index);
	// 	program = this->programs->find(this->programs, config->name);
	// 	if (program) {
	// 		program->update(config);
	// 		program->revised = true;
	// 	}
	// 	else {
	// 		this->new_program(config);
	// 	}
	// }


	// bool	supervisor_update(void)
// {
	// t_supervisor	*const this = supervisor();
	// t_uovector		*new_data;
	// t_program		*new_prog;
	// t_program		*old_prog;

	// //pass config data into supervisor structure
	// if (!parse_config_file(SUPERVISOR_CONFIG_FILE)) {
	// 	ERROR_SEND;
	// 	return (false);
	// }

	// new_data = new_uovector(sizeof(t_program));
	// vector_custom(this->programs, NULL, program_name_compare, NULL);
	// fill_vector(new_data);

	// this->programs->for_each(this->programs, set_program_unrevised);
	// for (size_t index = 0; index < new_data->size; index++)
	// {
	// 	new_prog = new_data->at(new_data, index);
	// 	old_prog = this->programs->find(this->programs, new_prog->name);
	// 	if (!old_prog)
	// 		old_prog = this->programs->push(this->programs, new_prog);
	// 	old_prog->sync(old_prog, new_prog);
	// 	old_prog->revised = true;
	// }
	// // make for_each_with(), receives an arg to pass along
	// // then this loop and the next call can also be for_each calls

	// delete_vector(new_data);
	// supervisor_execute(this);
// }

//programs are 