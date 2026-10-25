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

bool		pollable_read(t_pollable *const this);
int			pollable_send(t_pollable *const this,
				t_ipc_message *const response);
void		pollable_destroy(void *const pollable);
void		pollable_clear_data(t_pollable *const this);
void		pollable_do_nothing(void *source);
t_pollable	pollable_init(void);

#endif
