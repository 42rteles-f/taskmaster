#include <stdbool.h>
#include <string.h>
#include <yaml.h>

#include "parser_internal.h"

void	yaml_state_destroy(t_yaml_state *this)
{
	if (this == NULL)
		return ;
	if (this->document_loaded)
		yaml_document_delete(&this->document);
	if (this->parser_initialized)
		yaml_parser_delete(&this->parser);
	if (this->file != NULL)
		fclose(this->file);
	*this = (t_yaml_state){0};
}

bool	yaml_state_init(t_yaml_state *const this, const char *path)
{
	if (this == NULL || path == NULL)
		return (false);
	*this = (t_yaml_state){0};
	this->file = fopen(path, "rb");
	if (this->file == NULL || !yaml_parser_initialize(&this->parser))
		goto error;
	this->parser_initialized = true;

	yaml_parser_set_input_file(&this->parser, this->file);
	if (!yaml_parser_load(&this->parser, &this->document))
		goto error;
	this->document_loaded = true;

	return (true);
error:
	yaml_state_destroy(this);
	return (false);
}

yaml_node_t	*yaml_node_by_key(yaml_document_t *document,
		yaml_node_t *mapping, const char *wanted)
{
	yaml_node_t	*key;

	if (document == NULL || mapping == NULL || wanted == NULL
		|| mapping->type != YAML_MAPPING_NODE)
		return (NULL);

	for (
		yaml_node_pair_t *pair = mapping->data.mapping.pairs.start;
		pair < mapping->data.mapping.pairs.top;
		pair++
	) {
		key = yaml_document_get_node(document, pair->key);
		if (key != NULL && key->type == YAML_SCALAR_NODE
			&& strcmp((char *)key->data.scalar.value, wanted) == 0)
		{
			return (yaml_document_get_node(document, pair->value));
		}
	}
	return (NULL);
}

const char	*yaml_value_by_key(yaml_document_t *document,
		yaml_node_t *mapping, const char *wanted)
{
	yaml_node_t	*value;

	value = yaml_node_by_key(document, mapping, wanted);
	if (value == NULL || value->type != YAML_SCALAR_NODE)
		return (NULL);
	return (value->data.scalar.value);
}
