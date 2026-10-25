#ifndef PARSER_H
# define PARSER_H

# include <program.h>
# include <yaml.h>

void		populate_config(t_config *config, yaml_document_t *document,
				yaml_node_t *program);
t_uovector	*parse_yaml_file(const char *path);

#endif
