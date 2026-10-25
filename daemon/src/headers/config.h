#ifndef CONFIG_H
# define CONFIG_H

# include <stdbool.h>
# include <stddef.h>
# include <indexmap.h>

# define CONFIG_EXIT_COUNT 256

typedef enum e_autostart {
	AUTOSTART_INVALID = -1,
	AUTOSTART_FALSE,
	AUTOSTART_TRUE
} t_autostart;

typedef enum e_autorestart {
	AUTORESTART_INVALID = -1,
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
	t_autostart		autostart;
	t_autorestart	autorestart;
	bool			exitcodes[CONFIG_EXIT_COUNT];
	unsigned int	startretries;
	unsigned int	starttime;
	int				stopsignal;
	unsigned int	stoptime;
	char			*stdout_path;
	char			*stderr_path;
	t_strmap		env;
}	t_config;

void		config_destroy(void *element);
t_config	config_init(void);
void		copy_string_from_scalar(const char *source, char **destination);
void		copy_unsigned_from_scalar(const char *source,
										  unsigned int *destination);
void		copy_mode_from_scalar(const char *source, mode_t *destination);
void		copy_autostart_from_scalar(const char *source,
									 t_autostart *destination);
void		copy_autorestart_from_scalar(const char *source,
											 t_autorestart *destination);
void		copy_signal_from_scalar(const char *source, int *destination);
bool		config_validate(t_config *cfg);


#endif




