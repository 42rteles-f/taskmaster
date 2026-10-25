#include <parser.h>
#include "parser_internal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <yaml.h>

static void	copy_exitcodes_from_sequence(yaml_document_t *document,
		bool destination[CONFIG_EXIT_COUNT], yaml_node_t *source);
static void	copy_environment_from_mapping(yaml_document_t *document,
		t_strmap *destination, yaml_node_t *source);

void	populate_config(t_config *config, yaml_document_t *document,
		yaml_node_t *program)
{
	const char	*value;
	yaml_node_t	*node;

	*config = config_init();
	value = yaml_value_by_key(document, program, "cmd");
	copy_string_from_scalar(value, &config->cmd);
	value = yaml_value_by_key(document, program, "numprocs");
	copy_unsigned_from_scalar(value, &config->numprocs);
	value = yaml_value_by_key(document, program, "umask");
	copy_mode_from_scalar(value, &config->umask);
	value = yaml_value_by_key(document, program, "workingdir");
	copy_string_from_scalar(value, &config->workingdir);
	value = yaml_value_by_key(document, program, "autostart");
	copy_autostart_from_scalar(value, &config->autostart);
	value = yaml_value_by_key(document, program, "autorestart");
	copy_autorestart_from_scalar(value, &config->autorestart);
	node = yaml_node_by_key(document, program, "exitcodes");
	copy_exitcodes_from_sequence(document, config->exitcodes, node);
	value = yaml_value_by_key(document, program, "startretries");
	copy_unsigned_from_scalar(value, &config->startretries);
	value = yaml_value_by_key(document, program, "starttime");
	copy_unsigned_from_scalar(value, &config->starttime);
	value = yaml_value_by_key(document, program, "stopsignal");
	copy_signal_from_scalar(value, &config->stopsignal);
	value = yaml_value_by_key(document, program, "stoptime");
	copy_unsigned_from_scalar(value, &config->stoptime);
	value = yaml_value_by_key(document, program, "stdout");
	copy_string_from_scalar(value, &config->stdout_path);
	value = yaml_value_by_key(document, program, "stderr");
	copy_string_from_scalar(value, &config->stderr_path);
	node = yaml_node_by_key(document, program, "env");
	copy_environment_from_mapping(document, &config->env, node);
}

static void	copy_exitcodes_from_sequence(yaml_document_t *document,
										 bool destination[CONFIG_EXIT_COUNT],
										 yaml_node_t *source)
{
	yaml_node_t		*value;
	unsigned int	code;

	if (source == NULL || destination == NULL
		|| source->type != YAML_SEQUENCE_NODE)
		return ;

	memset(destination, 0, sizeof(bool) * CONFIG_EXIT_COUNT);

	for (
		yaml_node_item_t *item = source->data.sequence.items.start;
		item < source->data.sequence.items.top;
		item++
	) {
		value = yaml_document_get_node(document, *item);
		if (value != NULL && value->type == YAML_SCALAR_NODE)
		{
			code = (unsigned int)strtoul((char *)value->data.scalar.value,
					NULL, 10);
			if (code < CONFIG_EXIT_COUNT)
				destination[code] = true;
		}
	}
}

static void	copy_environment_from_mapping(yaml_document_t *document,
										  t_strmap *destination,
										  yaml_node_t *source)
{
	yaml_node_t	*key;
	yaml_node_t	*value;

	if (source == NULL || destination == NULL
		|| source->type != YAML_MAPPING_NODE)
		return ;

	for (
		yaml_node_pair_t *pair = source->data.mapping.pairs.start;
		pair < source->data.mapping.pairs.top;
		pair++
	) {
		key = yaml_document_get_node(document, pair->key);
		value = yaml_document_get_node(document, pair->value);
		if (key != NULL && value != NULL && key->type == YAML_SCALAR_NODE
			&& value->type == YAML_SCALAR_NODE
		) {
			destination->set(destination, key->data.scalar.value,
				value->data.scalar.value);
		}
	}
}

static t_uovector	*extract_configs(yaml_node_t *programs, t_yaml_state *state)
{
	t_uovector	*vec;
	yaml_node_t	*name;
	yaml_node_t	*program;
	t_config	config;

	vec = new_uovector(sizeof(t_config));
	vector_custom(vec, NULL, NULL, config_destroy);
	for (
		yaml_node_pair_t *pair = programs->data.mapping.pairs.start;
		pair < programs->data.mapping.pairs.top;
		pair++
	) {
		name = yaml_document_get_node(&state->document, pair->key);
		program = yaml_document_get_node(&state->document, pair->value);
		if (name == NULL || name->type != YAML_SCALAR_NODE
			|| program == NULL || program->type != YAML_MAPPING_NODE
		) {
			delete_vector(vec);
			return (NULL);
		}
		populate_config(&config, &state->document, program);
		config.name = strdup((char *)name->data.scalar.value);
		if (!config_validate(&config))
		{
			config_destroy(&config);
			delete_vector(vec);
			return (NULL);
		}
		vec->push(vec, &config);
	}
	return (vec);
}

t_uovector	*parse_yaml_file(const char *path)
{
	t_yaml_state	state;
	yaml_node_t		*root;
	yaml_node_t		*programs;
	t_uovector		*configs;

	if (!yaml_state_init(&state, path))
		return (NULL);

	root = yaml_document_get_root_node(&state.document);
	if (root == NULL || root->type != YAML_MAPPING_NODE)
		goto error;

	programs = yaml_node_by_key(&state.document, root, "programs");
	if (programs == NULL || programs->type != YAML_MAPPING_NODE)
		goto error;

	configs = extract_configs(programs, &state);
	yaml_state_destroy(&state);

	return (configs);

error:
	yaml_state_destroy(&state);
	return (NULL);
}
