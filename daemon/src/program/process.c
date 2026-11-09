// Custom Headers
#include "process.h"
#include <indexmap.h>

// Standard Headers
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void	process_change_state(t_process *this, t_process_state state);

t_process	process_init(size_t slot_id, const t_config *config)
{
	return ((t_process){
		.slot_id = slot_id,
		.config = config,
		.pid = -1,
		.state = PROCESS_STOPPED,
	});
}

void	process_destroy(void *element)
{
	t_process	*process;

	process = element;
	process_force_stop(process);
	*process = (t_process){.pid = -1};
}

void	process_stop(t_process *this)
{
	if (this->pid <= 0 ||
		PROCESS_STOPPING == this->state ||
		PROCESS_KILLING == this->state)
		return ;

	if (kill(this->pid, this->config->stopsignal) == 0)
		process_change_state(this, PROCESS_STOPPING);
	else if (errno == ESRCH)
		process_change_state(this, PROCESS_KILLING);
}

void	process_force_stop(t_process *this)
{
	char *reason;
    int status;

	if (PROCESS_KILLING == this->state || this->pid <= 0)
        return;

	if (kill(this->pid, SIGKILL) == 0 ||
		errno == ESRCH)
	{
        process_change_state(this, PROCESS_KILLING);
	}
    else
	{
		reason = strerror(errno);
		//TODO logger;
	}
}

static void	process_change_state_at(t_process *this, t_process_state state, struct timespec *now)
{
	if (state == this->state)
		return ;
		
	this->state = state;
	this->state_at = *now;

	this->deadline = this->state_at;
    if (PROCESS_STARTING == state)
		this->deadline.tv_sec += this->config->starttime;
	else if (PROCESS_STOPPING == state)
		this->deadline.tv_sec += this->config->stoptime;
	//TODO logger here
}

static void	process_change_state(t_process *this, t_process_state state)
{
	if (state == this->state)
		return ;
		
	this->state = state;
	clock_gettime(CLOCK_MONOTONIC, &this->state_at);

	this->deadline = this->state_at;
    if (PROCESS_STARTING == state)
		this->deadline.tv_sec += this->config->starttime;
	else if (PROCESS_STOPPING == state)
		this->deadline.tv_sec += this->config->stoptime;
	//TODO logger here
}

static bool	redirect_output(const char *path, int target_fd)
{
	int	fd;

	if (!path)
		return (true);
	fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0666);
	if (fd < 0)
		return (false);
	if (dup2(fd, target_fd) < 0)
	{
		close(fd);
		return (false);
	}
	close(fd);
	return (true);
}

static bool	apply_environment(const t_strmap *env)
{
	t_pair	pair;

	for (size_t index = 0; index < env->keys.size; index++)
	{
		pair = env->at(env, index);
		if (setenv(*(char **)pair.first, *(char **)pair.second, 1) < 0)
			return (false);
	}
	return (true);
}

static void	reset_child_signals(void)
{
	struct sigaction	action;
	sigset_t			empty;

	action = (struct sigaction){.sa_handler = SIG_DFL};
	sigemptyset(&action.sa_mask);
	sigemptyset(&empty);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGQUIT, &action, NULL);
	sigaction(SIGHUP, &action, NULL);
	sigaction(SIGCHLD, &action, NULL);
	sigprocmask(SIG_SETMASK, &empty, NULL);
}

static void	child_execute(const t_config *config)
{
	reset_child_signals();
	umask(config->umask);
	if (config->workingdir && chdir(config->workingdir) < 0)
		_exit(126);
	if (!redirect_output(config->stdout_path, STDOUT_FILENO) ||
		!redirect_output(config->stderr_path, STDERR_FILENO) ||
		!apply_environment(&config->env))
	{
		_exit(126);
	}
	execl("/bin/sh", "sh", "-c", config->cmd, (char *)NULL);
	_exit(127);
}

bool	process_start(t_process *this)
{
	if (!this->config)
		return (false);

	this->pid = fork();
	if (0 > this->pid) {
		process_change_state(this, PROCESS_FATAL);
		return (false);
	}

	if (this->pid == 0)
		child_execute(this->config);

	this->wait_status = 0;
	process_change_state(this, PROCESS_STARTING);
	return (true);
}

int	process_compare_pid(const void *first, const void *second, size_t size)
{
	(void)size;
	return (((t_process*)first)->pid - *(int*)second);
}

void process_handle_exit(t_process *this, int status, struct timespec *now)
{
	const t_process_state	current_state = this->state;

	if (PROCESS_STOPPING == current_state ||
		PROCESS_KILLING == current_state)
	{
		this->wait_status = status;
		process_change_state_at(this, PROCESS_STOPPED, now);
		return ;
	}

	process_change_state_at(this, PROCESS_BACKOFF, now);
	if (current_state == this->state)
	{
		if (this->retries > this->config->startretries)
		{
			process_change_state_at(this, PROCESS_FATAL, now);
			// TODO log max retries.
		}
		else
		{
			process_start(this);
			this->retries++;
		}
	}
	else
	{
		this->retries = 0;
		process_start(this);
	}
}

void	process_update(t_process *this, struct timespec *now)
{
	if (PROCESS_FATAL == this->state || now->tv_sec < this->deadline.tv_sec)
		return ;

    if (PROCESS_STARTING == this->state)
		process_change_state_at(this, PROCESS_RUNNING, now);
	else if (PROCESS_STOPPING == this->state)
		process_force_stop(this);
}
