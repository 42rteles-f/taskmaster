#include "vector.h"

void	vector_custom(t_vector *this, t_vector_copy copy,
		t_vector_compare compare, t_vector_destroy_element destroy)
{
	if (copy)
		this->copy = copy;
	if (compare)
		this->compare = compare;
	this->destroy_element = destroy;
}

void	vector_destroy(t_vector *this)
{
	size_t	index;

	index = 0;
	while (this->destroy_element && index < this->size)
	{
		this->destroy_element(vector_at(this, index));
		index++;
	}
	free(this->data);
	this->data = NULL;
	this->size = 0;
	this->capacity = 0;
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
	if (!size)
		return ((t_vector){0});
	return ((t_vector) {
		.data = NULL,
		.size = 0,
		.capacity = 0,
		.element_size = size,
		
		.compare = memcmp,
		.copy = memcpy,
		.destroy_element = NULL,
		.push = vector_push_back,
		.get_index = vector_get_index,
		.insert = vector_insert,
		.emplace = vector_emplace,
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

t_vector	*new_vector(size_t size)
{
	t_vector	*new;

	if (!size)
		return (NULL);
	new = malloc(sizeof(t_vector));
	*new = init_vector(size);
	return (new);
}

t_vector	init_uovector(size_t size)
{
	t_vector	init;

	init = init_vector(size);
	init.remove_at = vector_mix_remove_index;
	init.remove_element = vector_mix_remove_element;
	return (init);
}

t_uovector	*new_uovector(size_t size)
{
	t_uovector	*new;

	new = malloc(sizeof(t_uovector));
	*new = init_uovector(size);
	return (new);
}

t_pvector	*new_pvector(size_t size)
{
	t_vector	*new;

	new = malloc(sizeof(t_pvector));
	*new = init_vector(size);
	return (new);
}
