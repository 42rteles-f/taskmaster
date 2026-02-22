#include "vector.h"

inline void	*vector_end(const t_vector *const this)
{
	return (this->data + this->size * this->element_size);
}

inline void	*vector_at(const t_vector *const this, const size_t find)
{
	if (find >= this->size)
		return (NULL);
	return (this->data + find * this->element_size);
}

void	*vector_search(const t_vector *const this, const void *const find)
{
	void	*const end = vector_end(this);

	for (void *it = this->data; it < end; it += this->element_size)
	{
		if (this->compare(it, find, this->element_size) == 0)
			return (it);
	}
	return (NULL);
}

int	vector_get_index(const t_vector *const this, const void *const find)
{
	void	*const end = vector_end(this);

	for (void *it = this->data; it < end; it += this->element_size)
	{
		if (this->compare(it, find, this->element_size) == 0) {
			return ((it - this->data) / this->element_size);
		}
	}
	return (-1);
}

void	*vector_search_with(const t_vector *const this,
							const void *const find,
							int (*comparator)(const void*, const void*))
{
	void	*const end = vector_end(this);

	for (void *it = this->data; it < end; it += this->element_size)
	{
		if (comparator(it, find) == 0)
			return (it);
	}
	return (NULL);
}
