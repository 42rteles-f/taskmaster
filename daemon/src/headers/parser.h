#ifndef PARSER_H
# define PARSER_H

# include <program.h>
# include <yaml.h>

t_uovector	*parse_yaml_file(const char *path);
void		config_destroy(void *element);
void		*copy_value_from_program(yaml_node_t *program, char *key,
				void *dest, size_t size);
void		*set_value_from_program(yaml_node_t *program, char *key,
				void *dest, size_t size);
t_config	extract_config(yaml_node_t *program);

#endif
