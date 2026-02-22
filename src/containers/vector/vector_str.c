#include "vector.h"

void	*vector_custom_strcpy(void *dest, const void *src, size_t)
{
	*(void**)dest = strdup(src);
	return (dest);
}

int	vector_custom_strcmp(const void *dest, const void *src, size_t)
{
	return (strcmp(*(void**)dest, src));
}

void	vector_custom_strdestroy(t_vector *this)
{
	void	**const data = this->data;

	for (size_t index = 0; index < this->size; index++)
		free(data[index]);
	vector_destroy(this);
}

void	vector_strigfy(t_vector *this)
{
	this->copy = vector_custom_strcpy;
	this->compare = vector_custom_strcmp;
	this->destroy = vector_custom_strdestroy;
}
 