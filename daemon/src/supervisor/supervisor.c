// Custom Headers
#include <supervisor.h>
#include <program.h>
#include <ipc.h>
#include <server.h>
#include <vector.h>
#include "supervisor_internal.h"

// Standard Headers
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>

static t_supervisor	this = {
	.signal_pipe = {-1, -1},
	.init = supervisor_init,
	.handle_request = supervisor_handle_request,
	.sync = supervisor_sync,
	.destroy = supervisor_destroy
};

t_supervisor	*supervisor(void)
{
	return (&this);
}

bool	supervisor_init(void)
{
	this.programs = new_uovector(sizeof(t_program));
	vector_custom(this.programs, NULL, program_cmp_str_name, program_destroy);

	this.commands[IPC_STATUS] = supervisor_handle_status;
	this.commands[IPC_START] = supervisor_handle_start;
	this.commands[IPC_STOP] = supervisor_handle_stop;
	this.commands[IPC_RESTART] = supervisor_handle_restart;
	this.commands[IPC_RELOAD] = supervisor_handle_reload;
	this.commands[IPC_SHUTDOWN] = supervisor_handle_shutdown;

	if (!supervisor_setup_pipe() ||
		!supervisor_set_signals())
	{
		supervisor_destroy();
		return (false);
	}
	return (true);
}

void	supervisor_destroy(void)
{
	if (this.signal_pipe[0] >= 0) close(this.signal_pipe[0]);
	if (this.signal_pipe[1] >= 0) close(this.signal_pipe[1]);
	this.signal_pipe[0] = -1;
	this.signal_pipe[1] = -1;
	delete_vector(this.programs);
	this.programs = NULL;
}


void	call_program_execute(void *element, int)
{
	((t_program*)element)->execute(element);
}

void	set_program_unrevised(void *element)
{
	((t_program*)element)->revised = false;
}

static void	supervisor_remove_unrevised(void)
{
	size_t		index;
	t_program	*program;

	index = 0;
	while (index < this.programs->size)
	{
		program = this.programs->at(this.programs, index);
		if (false == program->revised) {
			printf("removing program: %s\n", program->config.name);
			this.programs->remove_at(this.programs, index);
		}
		else
			index++;
	}
}

void	supervisor_update_program(void *config)
{
	t_program	*program;

	program = this.programs->find(this.programs, ((t_config*)config)->name);
	if (!program) {
		program = this.programs->emplace(this.programs);
		*program = program_init();
	}

	program->update(program, config);
	program->revised = true;
}

bool	supervisor_sync(t_uovector *configs)
{
	if (!configs) return (false);

	this.programs->for_each(this.programs, set_program_unrevised);
	configs->for_each(configs, supervisor_update_program);
	supervisor_remove_unrevised();
	delete_vector(configs);
	return (true);
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
	// vector_custom(this->programs, NULL, program_cmp_str_name, NULL);
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
