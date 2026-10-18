#include <parser.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

static yaml_document_t	*g_document;
static const char		*g_program_name;

static yaml_node_t	*mapping_value(yaml_node_t *mapping, const char *wanted)
{
	yaml_node_pair_t	*pair;
	yaml_node_t		*key;

	if (g_document == NULL || mapping == NULL
		|| mapping->type != YAML_MAPPING_NODE)
		return (NULL);
	pair = mapping->data.mapping.pairs.start;
	while (pair < mapping->data.mapping.pairs.top)
	{
		key = yaml_document_get_node(g_document, pair->key);
		if (key != NULL && key->type == YAML_SCALAR_NODE
			&& strcmp((char *)key->data.scalar.value, wanted) == 0)
			return (yaml_document_get_node(g_document, pair->value));
		pair++;
	}
	return (NULL);
}

static const char	*yaml_value_by_key(yaml_node_t *mapping,
		const char *wanted)
{
	yaml_node_t	*value;

	value = mapping_value(mapping, wanted);
	if (value == NULL || value->type != YAML_SCALAR_NODE)
		return (NULL);
	return ((const char *)value->data.scalar.value);
}

static void	copy_signal_from_scalar(int *destination, const char *source)
{
	static const struct {
		const char	*name;
		int			number;
	} signals[] = {
		{"HUP", SIGHUP}, {"INT", SIGINT}, {"QUIT", SIGQUIT},
		{"KILL", SIGKILL}, {"USR1", SIGUSR1}, {"USR2", SIGUSR2},
		{"TERM", SIGTERM}, {"CONT", SIGCONT}, {"STOP", SIGSTOP},
		{"TSTP", SIGTSTP}
	};
	size_t	i;

	if (source == NULL || destination == NULL)
		return ;
	*destination = -1;
	i = 0;
	while (i < sizeof(signals) / sizeof(signals[0]))
	{
		if (strcmp(source, signals[i].name) == 0)
		{
			*destination = signals[i].number;
			return ;
		}
		i++;
	}
}

void	*copy_value_from_program(yaml_node_t *program, char *key,
		void *dest, size_t size)
{
	yaml_node_t	*value;
	const char	*source;
	char		*copy;

	if (program == NULL || key == NULL || dest == NULL
		|| size != sizeof(char *))
		return (NULL);
	if (strcmp(key, "name") == 0)
		source = g_program_name;
	else
	{
		value = mapping_value(program, key);
		if (value == NULL)
			return (dest);
		if (value->type != YAML_SCALAR_NODE)
			return (NULL);
		source = (char *)value->data.scalar.value;
	}
	if (source == NULL)
		return (NULL);
	copy = strdup(source);
	if (copy == NULL)
		return (NULL);
	memcpy(dest, &copy, sizeof(copy));
	return (dest);
}

void	set_value_from_program(yaml_node_t *program, char *key,
		void *dest, size_t size)
{
	yaml_node_t		*value;
	t_autorestart	restart;
	int				signal;
	unsigned int	number;
	const char		*scalar;

	if (program == NULL || key == NULL || dest == NULL)
		return ;
	value = mapping_value(program, key);
	if (value == NULL || value->type != YAML_SCALAR_NODE)
		return ;

	scalar = (char *)value->data.scalar.value;
	if (strcmp(key, "autostart") == 0 && size == sizeof(bool))
		*(bool*)dest = (strcmp(scalar, "true") == 0);
	else if (
		strcmp(key, "autorestart") == 0
		&& sizeof(t_autorestart) == size
	) {
		restart = AUTORESTART_UNEXPECTED;
		if (strcmp(scalar, "never") == 0 || strcmp(scalar, "false") == 0)
			restart = AUTORESTART_NEVER;
		else if (strcmp(scalar, "always") == 0
			|| strcmp(scalar, "true") == 0)
			restart = AUTORESTART_ALWAYS;
		memcpy(dest, &restart, size);
	}
	else if (strcmp(key, "stopsignal") == 0 && size == sizeof(int))
	{
		copy_signal_from_scalar(&signal, scalar);
		memcpy(dest, &signal, size);
	}
	else
	{
		if (strcmp(key, "umask") == 0)
			number = (unsigned int)strtoul(scalar, NULL, 8);
		else
			number = (unsigned int)strtoul(scalar, NULL, 10);
		memcpy(dest, &number, size);
	}
}

static void	set_exitcodes(yaml_node_t *program, t_config *config)
{
	yaml_node_t		*sequence;
	yaml_node_item_t	*item;
	yaml_node_t		*value;
	unsigned int	code;

	sequence = mapping_value(program, "exitcodes");
	if (sequence == NULL || sequence->type != YAML_SEQUENCE_NODE)
		return ;
	memset(config->exitcodes, 0, sizeof(config->exitcodes));
	item = sequence->data.sequence.items.start;
	while (item < sequence->data.sequence.items.top)
	{
		value = yaml_document_get_node(g_document, *item);
		if (value != NULL && value->type == YAML_SCALAR_NODE)
		{
			code = (unsigned int)strtoul((char *)value->data.scalar.value,
					NULL, 10);
			if (code < 256)
				config->exitcodes[code] = true;
		}
		item++;
	}
}

static void	set_environment(yaml_node_t *program, t_config *config)
{
	yaml_node_t		*environment;
	yaml_node_pair_t	*pair;
	yaml_node_t		*key;
	yaml_node_t		*value;

	environment = mapping_value(program, "env");
	if (environment == NULL || environment->type != YAML_MAPPING_NODE)
		return ;
	pair = environment->data.mapping.pairs.start;
	while (pair < environment->data.mapping.pairs.top)
	{
		key = yaml_document_get_node(g_document, pair->key);
		value = yaml_document_get_node(g_document, pair->value);
		if (key != NULL && value != NULL && key->type == YAML_SCALAR_NODE
			&& value->type == YAML_SCALAR_NODE)
			config->env.set(&config->env, key->data.scalar.value,
				value->data.scalar.value);
		pair++;
	}
}

void	config_destroy(void *element)
{
	t_config	*config;

	config = element;
	free(config->name);
	free(config->cmd);
	free(config->workingdir);
	free(config->stdout_path);
	free(config->stderr_path);
	config->env.destroy(&config->env);
	*config = (t_config){0};
}

static t_config	config_default(void)
{
	t_config	config;

	config = (t_config){
		.numprocs = 1,
		.umask = 022,
		.autostart = true,
		.autorestart = AUTORESTART_UNEXPECTED,
		.startretries = 3,
		.starttime = 1,
		.stopsignal = SIGTERM,
		.stoptime = 10,
		.env = init_strmap(sizeof(char *))
	};
	config.exitcodes[0] = true;
	vector_strigfy(&config.env.values);
	return (config);
}

t_config	extract_config_legacy(yaml_node_t *program)
{
	t_config	config;

	config = config_default();
	copy_value_from_program(program, "name", &config.name,
		sizeof(config.name));
	copy_value_from_program(program, "cmd", &config.cmd, sizeof(config.cmd));
	set_value_from_program(program, "numprocs", &config.numprocs,
		sizeof(config.numprocs));
	set_value_from_program(program, "umask", &config.umask,
		sizeof(config.umask));
	copy_value_from_program(program, "workingdir", &config.workingdir,
		sizeof(config.workingdir));
	set_value_from_program(program, "autostart", &config.autostart,
		sizeof(config.autostart));
	set_value_from_program(program, "autorestart", &config.autorestart,
		sizeof(config.autorestart));
	set_exitcodes(program, &config);
	set_value_from_program(program, "startretries", &config.startretries,
		sizeof(config.startretries));
	set_value_from_program(program, "starttime", &config.starttime,
		sizeof(config.starttime));
	set_value_from_program(program, "stopsignal", &config.stopsignal,
		sizeof(config.stopsignal));
	set_value_from_program(program, "stoptime", &config.stoptime,
		sizeof(config.stoptime));
	copy_value_from_program(program, "stdout", &config.stdout_path,
		sizeof(config.stdout_path));
	copy_value_from_program(program, "stderr", &config.stderr_path,
		sizeof(config.stderr_path));
	set_environment(program, &config);
	return (config);
}

static void	copy_string_from_scalar(char **destination, const char *source)
{
	char	*copy;

	if (source == NULL || destination == NULL)
		return ;
	copy = strdup(source);
	if (copy != NULL)
		*destination = copy;
}

static void	copy_unsigned_from_scalar(unsigned int *destination,
		const char *source)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (unsigned int)strtoul(source, NULL, 10);
}

static void	copy_mode_from_scalar(mode_t *destination, const char *source)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (mode_t)strtoul(source, NULL, 8);
}

static void	copy_bool_from_scalar(bool *destination, const char *source)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = (strcmp(source, "true") == 0);
}

static void	copy_autorestart_from_scalar(t_autorestart *destination,
		const char *source)
{
	if (source == NULL || destination == NULL)
		return ;
	*destination = AUTORESTART_UNEXPECTED;
	if (strcmp(source, "never") == 0 || strcmp(source, "false") == 0)
		*destination = AUTORESTART_NEVER;
	else if (strcmp(source, "always") == 0 || strcmp(source, "true") == 0)
		*destination = AUTORESTART_ALWAYS;
}

static void	copy_exitcodes_from_sequence(bool destination[256],
		yaml_node_t *source)
{
	yaml_node_item_t	*item;
	yaml_node_t		*value;
	unsigned int	code;

	if (source == NULL || destination == NULL
		|| source->type != YAML_SEQUENCE_NODE)
		return ;
	memset(destination, 0, sizeof(bool) * 256);
	item = source->data.sequence.items.start;
	while (item < source->data.sequence.items.top)
	{
		value = yaml_document_get_node(g_document, *item);
		if (value != NULL && value->type == YAML_SCALAR_NODE)
		{
			code = (unsigned int)strtoul((char *)value->data.scalar.value,
					NULL, 10);
			if (code < 256)
				destination[code] = true;
		}
		item++;
	}
}

static void	copy_environment_from_mapping(t_strmap *destination,
		yaml_node_t *source)
{
	yaml_node_pair_t	*pair;
	yaml_node_t		*key;
	yaml_node_t		*value;

	if (source == NULL || destination == NULL
		|| source->type != YAML_MAPPING_NODE)
		return ;
	pair = source->data.mapping.pairs.start;
	while (pair < source->data.mapping.pairs.top)
	{
		key = yaml_document_get_node(g_document, pair->key);
		value = yaml_document_get_node(g_document, pair->value);
		if (key != NULL && value != NULL && key->type == YAML_SCALAR_NODE
			&& value->type == YAML_SCALAR_NODE)
			destination->set(destination, key->data.scalar.value,
				value->data.scalar.value);
		pair++;
	}
}

t_config	extract_config(yaml_node_t *program)
{
	t_config	config;
	const char	*value;
	yaml_node_t	*node;

	config = config_default();
	copy_string_from_scalar(&config.name, g_program_name);

	value = yaml_document_get_value_by_key(program, "cmd");
	copy_string_from_scalar(&config.cmd, value);
	
	value = yaml_document_get_value_by_key(program, "numprocs");
	copy_unsigned_from_scalar(&config.numprocs, value);
	
	value = yaml_document_get_value_by_key(program, "umask");
	copy_mode_from_scalar(&config.umask, value);
	
	value = yaml_document_get_value_by_key(program, "workingdir");
	copy_string_from_scalar(&config.workingdir, value);
	
	value = yaml_document_get_value_by_key(program, "autostart");
	copy_bool_from_scalar(&config.autostart, value);
	
	value = yaml_document_get_value_by_key(program, "autorestart");
	copy_autorestart_from_scalar(&config.autorestart, value);
	
	node = mapping_value(program, "exitcodes");
	copy_exitcodes_from_sequence(config.exitcodes, node);
	
	value = yaml_document_get_value_by_key(program, "startretries");
	copy_unsigned_from_scalar(&config.startretries, value);
	
	value = yaml_document_get_value_by_key(program, "starttime");
	copy_unsigned_from_scalar(&config.starttime, value);
	
	value = yaml_document_get_value_by_key(program, "stopsignal");
	copy_signal_from_scalar(&config.stopsignal, value);
	
	value = yaml_document_get_value_by_key(program, "stoptime");
	copy_unsigned_from_scalar(&config.stoptime, value);
	
	value = yaml_document_get_value_by_key(program, "stdout");
	copy_string_from_scalar(&config.stdout_path, value);
	
	value = yaml_document_get_value_by_key(program, "stderr");
	copy_string_from_scalar(&config.stderr_path, value);
	node = mapping_value(program, "env");
	copy_environment_from_mapping(&config.env, node);
	return (config);
}

static t_uovector	*extract_configs(yaml_node_t *programs)
{
	t_uovector			*configs;
	yaml_node_pair_t	*pair;
	yaml_node_t			*name;
	yaml_node_t			*program;
	t_config			config;

	configs = new_uovector(sizeof(t_config));
	if (configs == NULL)
		return (NULL);
	vector_custom(configs, NULL, NULL, config_destroy);
	pair = programs->data.mapping.pairs.start;
	while (pair < programs->data.mapping.pairs.top)
	{
		name = yaml_document_get_node(g_document, pair->key);
		program = yaml_document_get_node(g_document, pair->value);
		if (name == NULL || name->type != YAML_SCALAR_NODE
			|| program == NULL || program->type != YAML_MAPPING_NODE)
		{
			delete_vector(configs);
			return (NULL);
		}
		g_program_name = (char *)name->data.scalar.value;
		config = extract_config(program);
		configs->push(configs, &config);
		pair++;
	}
	return (configs);
}

typedef struct {
    FILE			*file;
    yaml_parser_t	parser;
    yaml_document_t	document;
	bool			parser_initialized;
    bool			document_loaded;
} t_yaml_state;

static void yaml_state_cleanup(t_yaml_state *s)
{
    if (s == NULL)
        return;
    if (s->document_loaded)
        yaml_document_delete(&s->document);
	if (s->parser_initialized)
		yaml_parser_delete(&s->parser);
    if (s->file)
        fclose(s->file);
	*s = (t_yaml_state){0};
}

yaml_node_t	*get_yaml_value_node(const char *path, t_yaml_state *const state, const char *name)
{
	yaml_node_t		*root;
	yaml_node_t		*node;

	state->file = fopen(path, "rb");
	if (state->file == NULL || !yaml_parser_initialize(&state->parser))
		goto error;
	state->parser_initialized = true;

	yaml_parser_set_input_file(&state->parser, state->file);
	if (!yaml_parser_load(&state->parser, &state->document))
		goto error;
	state->document_loaded = true;

	root = yaml_document_get_root_node(&state->document);
	if (root == NULL || root->type != YAML_MAPPING_NODE)
		goto error;

	node = mapping_value(root, name);
	if (node == NULL || node->type != YAML_MAPPING_NODE)
		goto error;

	return (node);
error:
	yaml_state_cleanup(state);
	return (NULL);
}

t_uovector	*parse_yaml_file(const char *path)
{
	t_yaml_state	state;
	yaml_node_t		*programs;
	t_uovector		*configs;

	configs = NULL;
	state = (t_yaml_state){0};
	programs = get_yaml_value_node(path, &state, "programs");
	if (!programs)
		configs = extract_configs(programs);
	yaml_state_cleanup(&state);
	return (configs);
}

