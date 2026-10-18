#ifndef CONFIG_H
# define CONFIG_H

# include <stdbool.h>
# include <stddef.h>
# include <indexmap.h>

typedef enum e_autorestart {
	AUTORESTART_NEVER,
	AUTORESTART_ALWAYS,
	AUTORESTART_UNEXPECTED
} t_autorestart;

typedef struct s_config {
	char			*name;
	char			*cmd;
	unsigned int	numprocs;
	mode_t			umask;
	char			*workingdir;
	bool			autostart;
	t_autorestart	autorestart;
	bool			exitcodes[256];
	unsigned int	startretries;
	unsigned int	starttime;
	int				stopsignal;
	unsigned int	stoptime;
	char			*stdout_path;
	char			*stderr_path;
	t_strmap		env;
}	t_config;

#endif




