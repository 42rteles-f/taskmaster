#include <parser.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

static yaml_node_t	*yaml_document_get_node_by_key(yaml_node_t *mapping, const char *wanted)
{
	yaml_node_t	*key;

	for (
		yaml_node_pair_t *pair = mapping->data.mapping.pairs.start;
		pair < mapping->data.mapping.pairs.top;
		pair++
	) {
		key = yaml_document_get_node(g_document, pair->key);
		if (key != NULL && key->type == YAML_SCALAR_NODE
			&& strcmp((char *)key->data.scalar.value, wanted) == 0)
		{
			return (yaml_document_get_node(g_document, pair->value));
		}
	}
	return (NULL);
}

static char	*yaml_document_get_value_by_key(yaml_node_t *mapping, const char *wanted)
{
	yaml_node_t	*node;

	node = yaml_document_get_node_by_key(source, key);
	if (node == NULL || node->type != YAML_SCALAR_NODE)
		return (NULL);

	return (node->data.scalar.value);
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

static void	copy_exitcodes_from_sequence(yaml_node_t *source,
		bool destination[256])
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

static void	copy_environment_from_mapping(yaml_node_t *source,
		t_strmap *destination)
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


static void	set_exitcodes(yaml_node_t *program, t_config *config)
{
	yaml_node_t		*sequence;
	yaml_node_item_t	*item;
	yaml_node_t		*value;
	unsigned int	code;

	sequence = yaml_document_get_node_by_key(program, "exitcodes");
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
	yaml_node_t			*environment;
	yaml_node_pair_t	*pair;
	yaml_node_t			*key;
	yaml_node_t			*value;

	environment = yaml_document_get_node_by_key(program, "env");
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

t_config	extract_config(yaml_node_t *program, const char *name)
{
	t_config	config;
	const char	*value;
	yaml_node_t	*node;

	config = config_default();
	copy_string_from_scalar(value, &config.name);
	value = yaml_document_get_value_by_key(program, "cmd");
	copy_string_from_scalar(value, &config.cmd);
	value = yaml_document_get_value_by_key(program, "numprocs");
	copy_unsigned_from_scalar(value, &config.numprocs);
	value = yaml_document_get_value_by_key(program, "umask");
	copy_mode_from_scalar(value, &config.umask);
	value = yaml_document_get_value_by_key(program, "workingdir");
	copy_string_from_scalar(value, &config.workingdir);
	value = yaml_document_get_value_by_key(program, "autostart");
	copy_bool_from_scalar(value, &config.autostart);
	value = yaml_document_get_value_by_key(program, "autorestart");
	copy_autorestart_from_scalar(value, &config.autorestart);
	node = mapping_value(program, "exitcodes");
	copy_exitcodes_from_sequence(node, config.exitcodes);
	value = yaml_document_get_value_by_key(program, "startretries");
	copy_unsigned_from_scalar(value, &config.startretries);
	value = yaml_document_get_value_by_key(program, "starttime");
	copy_unsigned_from_scalar(value, &config.starttime);
	value = yaml_document_get_value_by_key(program, "stopsignal");
	copy_signal_from_scalar(value, &config.stopsignal);
	value = yaml_document_get_value_by_key(program, "stoptime");
	copy_unsigned_from_scalar(value, &config.stoptime);
	value = yaml_document_get_value_by_key(program, "stdout");
	copy_string_from_scalar(value, &config.stdout_path);
	value = yaml_document_get_value_by_key(program, "stderr");
	copy_string_from_scalar(value, &config.stderr_path);
	node = mapping_value(program, "env");
	copy_environment_from_mapping(node, &config.env);
	return (config);
}


static t_uovector	*extract_configs(yaml_node_t *programs, const t_yaml_state *state)
{
	t_uovector			*configs;
	yaml_node_t			*name;
	yaml_node_t			*program;
	t_config			config;

	configs = new_uovector(sizeof(t_config));
	vector_custom(configs, NULL, NULL, config_destroy);
	for (
		yaml_node_pair_t *pair = programs->data.mapping.pairs.start;
		pair < programs->data.mapping.pairs.top;
		pair++
	) {
		name = yaml_document_get_node(&state->document, pair->key);
		program = yaml_document_get_node(&state->document, pair->value);
		if (name == NULL || name->type != YAML_SCALAR_NODE
			|| program == NULL || program->type != YAML_MAPPING_NODE)
		{
			delete_vector(configs);
			return (NULL);
		}
		config = extract_config(program, name->data.scalar.value);
		configs->push(configs, &config);
	}
	return (configs);
}

t_uovector	*parse_yaml_file(const char *path)
{
	t_yaml_state	state;
	t_uovector		*configs;
	yaml_node_t		*programs;

	configs = extract_configs(programs, &state);

}
