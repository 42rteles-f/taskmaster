// Custom Headers
#include <program.h>
#include <config.h>
#include <indexmap.h>
#include "process.h"

// Standard Headers
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

t_program	program_init(void)
{
	t_program	this;

	this = (t_program) {
		.processes = init_uovector(sizeof(t_process)),
		.config = new_config(),
		.sync = program_sync,
		.update = program_update,
		.destroy = program_destroy,
	};
	vector_custom(&this.processes, NULL, process_compare_pid, process_destroy);
	return (this);
}

void	program_destroy(void *arg)
{
	t_program	*const this = arg;

	vector_destroy(&this->processes);
	delete_config(this->config);
	*this = (t_program){};
}

static bool	safe_strcmp(const char *current, const char *next)
{
	if (!current || !next)
		return (current != next);
	return (strcmp(current, next));
}

static bool	environment_changed(const t_strmap *current,
								const t_strmap *update)
{
	t_pair		current_pair;
	const char	*update_value;

	if (current->keys.size != update->keys.size)
		return (true);

	for (size_t index = 0; index < current->keys.size; index++)
	{
		current_pair = current->at(current, index);
		update_value = update->get(update,
								   *(char **)current_pair.first);
		if (!update_value || safe_strcmp(*(char**)current_pair.second, update_value))
			return (true);
	}
	return (false);
}

static bool	process_should_restart(const t_config *current,
								   const t_config *update)
{
	if (!current || !update)
		return (true);
	return (
		current->umask != update->umask ||
		safe_strcmp(current->cmd, update->cmd) ||
		safe_strcmp(current->workingdir, update->workingdir) ||
		safe_strcmp(current->stdout_path, update->stdout_path) ||
		safe_strcmp(current->stderr_path, update->stderr_path) ||
		environment_changed(&current->env, &update->env)
	);
}

static void	program_sync_processes(t_program *this)
{
	t_process	*process;

	while (this->processes.size < this->config->numprocs)
	{
		process = this->processes.emplace(&this->processes);
		*process = process_init(this->next_slot_id++, this->config);
		if (!process_start(process))
			process->state = PROCESS_FATAL;
	}
	while (this->processes.size > this->config->numprocs)
	{
		this->processes.remove_at(&this->processes, this->processes.size - 1);
	}
}

void	program_sync(t_program *this, t_config *new_config)
{
	if (process_should_restart(this->config, new_config))
		this->processes.clear(&this->processes);

	config_move(this->config, new_config);
	program_sync_processes(this);
}

int	program_cmp_str_name(const void *element, const void *name, size_t size)
{
	(void)size;
	return (strcmp(((t_program*)element)->config->name, name));
}

void	call_process_update(void *element, void *time, size_t index)
{
	(void)index;
	if (PROCESS_FATAL != ((t_process*)element)->state)
	{
		process_update(element, time);
	}
}

void	program_update(t_program *this, struct timespec *now)
{
	this->processes.for_each_ctx(&this->processes, call_process_update, now);
	//TODO check process state
}

void	program_handle_child_exit(t_program *this, pid_t pid, int status, struct timespec *now)
{
	t_process	*child;

	child = this->processes.find(&this->processes, &pid);
	if (!child) return ;

	process_handle_exit(child, status, now);
}
