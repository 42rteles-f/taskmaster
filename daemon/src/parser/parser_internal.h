#ifndef PARSER_INTERNAL_H
# define PARSER_INTERNAL_H

# include <stdbool.h>
# include <stdio.h>
# include <yaml.h>

typedef struct {
	FILE			*file;
	yaml_parser_t	parser;
	yaml_document_t	document;
	bool			parser_initialized;
	bool			document_loaded;
} t_yaml_state;

bool		yaml_state_init(t_yaml_state *const this, const char *path);
void		yaml_state_destroy(t_yaml_state *this);

yaml_node_t	*yaml_node_by_key(yaml_document_t *document,
				yaml_node_t *mapping, const char *wanted);
const char	*yaml_value_by_key(yaml_document_t *document,
				yaml_node_t *mapping, const char *wanted);

#endif
