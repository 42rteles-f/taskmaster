#ifndef SERVER_H
# define SERVER_H

// Custom Headers
# include <indexmap.h>
# include <ipc.h>

// Standard Headers
# include <stdbool.h>
# include <errno.h>
# include <poll.h>
# include <stdio.h>
# include <string.h>
# include <sys/un.h>
# include <unistd.h>

# define HERE write(1, "here\n", 6)
# define THERE write(1, "there\n", 7)
# define READSIZE	1024
# define ERROR_SEND printf("TEMP. Error. %s, %s, %d\n", \
		strerror(errno), __func__, __LINE__)

# define TRACE printf("%s:%d\n", __func__, __LINE__)

typedef struct  sockaddr_un	t_unsock;
typedef struct 	pollfd		t_pollfd;

typedef t_ipc_message	(*t_server_handler)(const t_ipc_message *);

typedef struct s_server
{
	int					socket;
	bool				initialized;
	bool				online;
	t_indexmap			*subscriptions;
	t_server_handler	request_handler;

	bool	(*init)(t_server_handler);
	bool	(*start)(void);
	bool	(*register_event)(int fd,
								 void *context,
								 void (*handler)(t_pollfd *pollfd, void *context));
	bool	(*unregister_event)(int fd);
	void	(*stop)(void);
	bool	(*destroy)(void);
}	t_server;

t_server	*server(void);
bool		server_start(void);
void		server_stop(void);
bool		server_init(t_server_handler handler);
bool		server_destroy(void);
bool		server_open_connections(t_server *this);
bool		server_register_event(int fd,
									 void *context,
									 void (*handler)(t_pollfd *pollfd, void *context));
bool		server_unregister_event(int fd);
#endif
