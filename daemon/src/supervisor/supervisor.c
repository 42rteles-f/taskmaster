#include <supervisor.h>

bool	supervisor_init(void)
{
	t_supervisor	*this = supervisor();
	//start sighandlers
	//start current programs loaded
}

bool	supervisor_update(t_config *config)
{
	t_supervisor	*this = supervisor();
	t_vector		*remove;
	t_pair			entry;

	//pass config data into supervisor structure
	if (!config)
		printf("Invalid config file. No changes. Not a proper logger");

	remove = new_vector(sizeof(char*));
	for (size_t index = 0; index < this->programs; index++)
	{
		process = this->programs->at(this->programs, index);
	}

	supervisor_execute(this);
}

bool	supervisor_set_signal(void)
{
	t_supervisor	*this = supervisor();
	
}

bool	supervisor_execute(t_supervisor *this)
{
	supervisor_
}
