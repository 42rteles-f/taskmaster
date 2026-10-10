#ifndef POLLABLE_H
# define POLLABLE_H

# include <poll.h>
# include <ipc.h>
# include <stdbool.h>

typedef struct pollfd t_pollfd;

typedef struct
{
	t_pollfd		*pollfd;
	t_ipc_message	message;
	void			*context;

	void			(*handler)(void*);
}	t_pollable;

void		pollable_clean_array(t_uovector *const clients);
bool		pollable_read_message(t_pollable *const this);
void		pollable_destroy(void *const client);
void		pollable_clear_data(t_pollable *const this);
t_pollable	pollable_init(int fd);

#endif
