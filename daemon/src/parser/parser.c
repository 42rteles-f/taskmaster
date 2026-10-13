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

static int	signal_number(const char *name)
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

	i = 0;
	while (i < sizeof(signals) / sizeof(signals[0]))
	{
		if (strcmp(name, signals[i].name) == 0)
			return (signals[i].number);
		i++;
	}
	return (-1);
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
	bool			boolean;
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
		signal = signal_number(scalar);
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

t_config	extract_config(yaml_node_t *program)
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

t_uovector	*parse_yaml_file(const char *path)
{
	yaml_parser_t	parser;
	yaml_document_t	document;
	yaml_node_t		*root;
	yaml_node_t		*programs;
	t_uovector		*configs;
	FILE			*file;
	bool			parser_initialized;
	bool			document_loaded;

	file = NULL;
	configs = NULL;
	parser_initialized = false;
	document_loaded = false;
	file = fopen(path, "rb");
	if (file == NULL || !yaml_parser_initialize(&parser))
		goto error;
	parser_initialized = true;
	yaml_parser_set_input_file(&parser, file);
	if (!yaml_parser_load(&parser, &document))
		goto error;
	document_loaded = true;
	fclose(file);
	file = NULL;
	g_document = &document;
	root = yaml_document_get_root_node(&document);
	if (root == NULL || root->type != YAML_MAPPING_NODE)
		goto error;
	programs = mapping_value(root, "programs");
	if (programs == NULL || programs->type != YAML_MAPPING_NODE)
		goto error;
	configs = extract_configs(programs);
	g_document = NULL;
	g_program_name = NULL;
	yaml_document_delete(&document);
	yaml_parser_delete(&parser);
	return (configs);
error:
	g_document = NULL;
	g_program_name = NULL;
	if (configs != NULL)
		delete_vector(configs);
	if (document_loaded)
		yaml_document_delete(&document);
	if (parser_initialized)
		yaml_parser_delete(&parser);
	if (file != NULL)
		fclose(file);
	return (NULL);
}
