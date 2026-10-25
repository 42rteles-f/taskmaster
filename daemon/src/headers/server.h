#ifndef SERVER_H
# define SERVER_H

# include <ipc.h>
# include <vector.h>
# include <indexmap.h>
# include <errno.h>
# include <poll.h>
# include <stdio.h>
# include <string.h>
# include <sys/un.h>


# define HERE write(1, "here\n", 6)
# define THERE write(1, "there\n", 7)
# define READSIZE	1024
# define ERROR_SEND printf("TEMP. Error. %s, %s, %d\n", \
		strerror(errno), __func__, __LINE__)

#define TRACE printf("%s:%d\n", __func__, __LINE__)
typedef struct  sockaddr_un	t_unsock;
typedef struct 	pollfd		t_pollfd;

typedef struct s_server
{
	int			socket;
	bool		initialized;
	bool		online;
	t_indexmap	*pollables;

	bool	(*start)(void);
	void	*(*init)();
	void	*(*end)();
	void	(*destroy)();
}	t_server;

t_server	*server(void);
bool		server_start(void);
bool		server_init(void);
bool		server_destroy(void);
bool		server_open_connections(t_server *this);
void		server_handle_pollable(void *const arg);
#endif
