#ifndef PARSER_INTERNAL_H
# define PARSER_INTERNAL_H

typedef struct {
	FILE			*file;
	yaml_parser_t	parser;
	yaml_document_t	document;
	bool			parser_init;
	bool			doc_init;
} t_yaml_state;

#endif