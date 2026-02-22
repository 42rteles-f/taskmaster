#include "indexmap.h"

t_strmap	*new_strmap(size_t value_size)
{
	t_indexmap	*new;

	new = malloc(sizeof(t_indexmap));
	*new = init_indexmap(sizeof(char*), value_size);
	vector_strigfy(&new->keys);

	return ((t_strmap*)new);
}