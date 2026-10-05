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

void	vector_custom_strdestroy(void *element)
{
	free(*(char **)element);
}

void	vector_strigfy(t_vector *this)
{
	vector_custom(this, vector_custom_strcpy, vector_custom_strcmp,
		vector_custom_strdestroy);
}
