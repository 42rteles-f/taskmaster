#include "vector.h"

void	vector_destroy(t_vector *this)
{
	free(this->data);
}

void	delete_vector(t_vector *this)
{
	this->destroy(this);
	free(this);
}

void	vector_expand(t_vector *this)
{
	this->capacity = (this->capacity == 0) ? VECTOR_CAPACITY : this->capacity * 2;
	this->data = realloc(this->data, this->element_size * this->capacity);
}

void	*vector_pointer_cpy(void *dest, void *src, size_t)
{
	*(void**)dest = src;
	return (dest);
}

t_vector	init_vector(size_t size)
{
	return ((t_vector) {
		.data = NULL,
		.size = 0,
		.capacity = 0,
		.element_size = size,
		
		.compare = memcmp,
		.copy = memcpy,
		.push = vector_push_back,
		.get_index = vector_get_index,
		.insert = vector_insert,
		.remove_at = vector_remove_index,
		.remove_element = vector_remove_element,
		.find = vector_search,
		.find_with = vector_search_with,
		.at = vector_at,
		.for_each = vector_for_each,
		.destroy = vector_destroy,
		.end = vector_end,
		.push_batch = vector_push_batch,
	});
}

t_vector	init_uvector(size_t size)
{
	t_vector	init;

	init = init_vector(size);
	init.remove_at = vector_mix_remove_index;
	init.remove_element = vector_mix_remove_element;
	return (init);
}

t_vector	*new_vector(size_t size)
{
	t_vector	*new;

	new = malloc(sizeof(t_vector));
	*new = init_vector(size);
	return (new);
}

t_uvector	*new_uvector(size_t size)
{
	t_uvector	*new;

	new = malloc(sizeof(t_uvector));
	*new = init_uvector(size);
	return (new);
}

t_pvector	*new_pvector(size_t size)
{
	t_vector	*new;

	new = malloc(sizeof(t_pvector));
	*new = init_vector(size);
	return (new);
}