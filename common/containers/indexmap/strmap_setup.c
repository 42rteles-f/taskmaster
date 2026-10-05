#include "indexmap.h"

t_strmap	init_strmap(size_t value_size)
{
	t_strmap	map;

	map = init_indexmap(sizeof(char *), value_size);
	vector_strigfy(&map.keys);
	return (map);
}

t_strmap	*new_strmap(size_t value_size)
{
	t_indexmap	*new;

	new = malloc(sizeof(t_indexmap));
	*new = init_strmap(value_size);

	return ((t_strmap*)new);
}
