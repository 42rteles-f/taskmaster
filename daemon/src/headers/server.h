#ifndef SERVER_H
# define SERVER_H

# include <ipc.h>
# include <vector.h>
# include <indexmap.h>
# include <sys/un.h>

# define READSIZE	1024
# define ERROR_SEND printf("TEMP. Error. %s, %s, %s\n", strerror(errno), __func__, __LINE__);

typedef struct  protoent	t_protocol;
typedef struct  sockaddr_un	t_unsock;
typedef struct 	pollfd		t_pollfd;

typedef struct s_server
{
	int			socket;
	bool		initialized;
	bool		online;
	t_uvector	*clients;

	void	*(*online)();
	void	*(*init)();
	void	*(*end)();
	void	(*destroy)();
}	t_server;

t_server	*server(void);

#endif