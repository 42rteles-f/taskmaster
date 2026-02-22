#include "indexmap.h"

void	delete_indexmap(t_indexmap *this)
{
	this->destroy(this);
	free(this);
}

void destroy_indexmap(t_indexmap *this)
{
	this->keys.destroy(&this->keys);
	this->values.destroy(&this->values);
}

t_indexmap	init_indexmap(size_t key_size, size_t value_size)
{
	return ((t_indexmap) {
		.keys = init_vector(key_size),
		.values = init_vector(value_size),
		.compare = NULL,
		.set = indexmap_set,
		.get = indexmap_get,
		.emplace = indexmap_emplace,
		.remove = indexmap_remove,
		.destroy = destroy_indexmap
	});
}

t_indexmap	*new_indexmap(size_t key_size, size_t value_size)
{
	t_indexmap	*new;

	new = malloc(sizeof(t_indexmap));
	*new = init_indexmap(key_size, value_size);
	return (new);
}