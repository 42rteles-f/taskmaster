#ifndef SERVER_H
# define SERVER_H

# include <ipc.h>
# include <vector.h>
# include <indexmap.h>

typedef struct s_client
{
	int		socket;
}	t_client;

typedef struct s_server
{
	int			socket;
	t_uvector	*clients;

	void	*(*online)();
	void	*(*init)();
	void	*(*end)();
	void	(*destroy)();
}	t_server;

t_server	*server(void);

#endif